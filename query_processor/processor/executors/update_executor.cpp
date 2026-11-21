#include "update_executor.hpp"

bool UpdateExecutor::Next(Row& out)
{
    if(hasUpdated) return false;

    std::vector<Update> updateList;
    Row row;

    std::unordered_set<std::int16_t> seenRIDS;

    std::vector<Field> schemaField = childExecutor->OutputSchema().getFieldList();
    while(childExecutor->Next(row))
    {
        std::unordered_set<std::size_t> usedColumns;
        if(row.source.value().tableName != tableName || seenRIDS.find(row.source.value().rid.tupleSlot) != seenRIDS.end())
        {
            throw std::invalid_argument("[Update Executor - Next]: Found row that is not from same table / has repeat RID");
        }
        std::vector<FieldVariant> updatedValues = row.tuple.getTupleValues();
        if(updatedValues.size() != schemaField.size()) throw std::invalid_argument("[Update Executor - Next]: column count mismatch");
        seenRIDS.insert(row.source.value().rid.tupleSlot);
        
        for(const UpdateExpression& expression : updateExpressions)
        {
            if(expression.columnIndex < 0 || expression.columnIndex >= row.tuple.getTupleValues().size())
            {
                throw std::out_of_range("[Updated Exector - Next]: Invalid column index");
            }

            if(usedColumns.find(expression.columnIndex) != usedColumns.end())
            {
                throw std::invalid_argument("[Update Executor - Next]: Duplicate column index used");
            }
            usedColumns.insert(expression.columnIndex);

            ExpressionVariant expressionResult = expression.updateExpression->Eval(row);

            const FieldVariant& tupleValue = row.tuple.getTupleValueIndex(expression.columnIndex);

            if( std::holds_alternative<int64_t>(expressionResult) && !std::holds_alternative<int64_t>(tupleValue) || 
                std::holds_alternative<std::string>(expressionResult) && !std::holds_alternative<std::string>(tupleValue))
            {
                throw std::invalid_argument("[Update Executor - Next]: Expression Result & update column type mismatch");
            }

            if(std::holds_alternative<bool>(expressionResult))
            {
                throw std::invalid_argument("[Update Executor - Next]: Expression Result should not be bool");
            }
            
            if(std::holds_alternative<std::string>(expressionResult))
            {
                if(!isValidString32(std::get<std::string>(expressionResult)))
                {
                    throw std::invalid_argument("[Update Executor - Next]: Expression Result is not valid String32");
                }
                updatedValues[expression.columnIndex] = std::get<std::string>(expressionResult);
            }
            else if(std::holds_alternative<int64_t>(expressionResult))
            {
                updatedValues[expression.columnIndex] = std::get<int64_t>(expressionResult);
            }

        }
        updateList.push_back({row.source->rid, Tuple(std::move(updatedValues))});
    }

    std::int64_t rows_updated = 0;
    for(const Update& update : updateList)
    {
        databasePointer->updateTuple(tableName, update.rid, update.updateTuple);
        ++rows_updated;
    }
    
    hasUpdated = true;
    
    out = Row{Tuple({rows_updated}), std::nullopt};
    return true;
}
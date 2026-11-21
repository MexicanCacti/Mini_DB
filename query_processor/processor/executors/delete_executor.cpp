#include "delete_executor.hpp"

bool DeleteExecutor::Next(Row& out)
{
    if(hasDeleted) return false;

    std::vector<Row> deleteRows;
    Row row;
    std::unordered_set<std::int16_t> seenRIDS;
    std::vector<Field> schemaField = childExecutor->OutputSchema().getFieldList();

    while(childExecutor->Next(row))
    {
        if(row.source.value().tableName != tableName || seenRIDS.find(row.source.value().rid.tupleSlot) != seenRIDS.end())
        {
            throw std::invalid_argument("[Delete Executor - Next]: Found row that is not from same table / has repeat RID");
        }
        std::vector<FieldVariant> rowTuple = row.tuple.getTupleValues();
        if(rowTuple.size() != schemaField.size()) throw std::invalid_argument("[Delete Executor - Next]: column count mismatch");
        for(std::size_t i = 0 ; i < schemaField.size(); ++i)
        {
            if(std::holds_alternative<int64_t>(rowTuple[i]) && schemaField[i].type != FieldType::INT64) throw std::invalid_argument("[Delete Executor - Next]: type mismatch");
            if(std::holds_alternative<std::string>(rowTuple[i]) && schemaField[i].type != FieldType::STRING32) throw std::invalid_argument("[Delete Executor - Next]: type mismatch");
        }
        deleteRows.push_back(row);
        seenRIDS.insert(row.source.value().rid.tupleSlot);
    }

    std::int64_t rows_deleted = 0;
    for(const Row& deleteRow : deleteRows)
    {
        databasePointer->deleteTuple(tableName, deleteRow.source.value().rid);
        ++rows_deleted;
    }
    
    hasDeleted = true;
    
    out = Row{Tuple({rows_deleted}), std::nullopt};
    return true;
}
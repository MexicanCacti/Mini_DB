#include "insert_executor.hpp"

bool InsertExecutor::Next(Row& out)
{
    if(hasInserted) return false;

    std::vector<Row> insertRows;
    Row row;


    std::vector<Field> schemaField = childExecutor->OutputSchema().getFieldList();

    while(childExecutor->Next(row))
    {
        std::vector<FieldVariant> rowTuple = row.tuple.getTupleValues();
        if(rowTuple.size() != schemaField.size()) throw std::invalid_argument("[Insert Executor - Next]: column count mismatch");
        for(std::size_t i = 0 ; i < schemaField.size(); ++i)
        {
            if(std::holds_alternative<int64_t>(rowTuple[i]) && schemaField[i].type != FieldType::INT64) throw std::invalid_argument("[Insert Executor - Next]: type mismatch");
            if(std::holds_alternative<std::string>(rowTuple[i]) && schemaField[i].type != FieldType::STRING32) throw std::invalid_argument("[Insert Executor - Next]: type mismatch");
        }
        insertRows.push_back(row);
    }

    std::int64_t rows_inserted = 0;
    for(const Row& insertRow : insertRows)
    {
        RID insertRID = databasePointer->insertTuple(tableName, insertRow.tuple);
        if(insertRID.tupleSlot == -1)
        {
            out = Row{Tuple({rows_inserted}), std::nullopt};
            throw std::invalid_argument("[Insert Executor - Next]: Insert failed. Table full");
        }
        if(insertRID.pageID.tableID == -1)
        {
            out = Row{Tuple({rows_inserted}), std::nullopt};
            throw std::invalid_argument("[Insert Executor - Next]: Insert failed. Table does not exist");
        }
        ++rows_inserted;
    }
    
    hasInserted = true;
    
    out = Row{Tuple({rows_inserted}), std::nullopt};
    return true;
}
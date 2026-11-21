#include <filesystem>
#include <memory>
#include <vector>
#include <stdexcept>
#include <vector>
#include <windows.h>
#include <iostream>

#include "executor.hpp"
#include "expression.hpp"
#include "column_expression.hpp"
#include "constant_expression.hpp"
#include "addition_expression.hpp"
#include "equal_expression.hpp"
#include "greater_than_expression.hpp"
#include "greater_than_equal_expression.hpp"
#include "and_expression.hpp"

#include "values_executor.hpp"
#include "insert_executor.hpp"
#include "update_executor.hpp"
#include "delete_executor.hpp"
#include "sequential_scan_executor.hpp"
#include "projection_executor.hpp"
#include "filter_executor.hpp"
#include "nested_loop_join_executor.hpp"
#include "aggregation_executor.hpp"
#include "limit_executor.hpp"
#include "sort_executor.hpp"
#include "database.hpp"


std::string directoryString = "./storage";
std::string fileString = "rids.txt";

namespace fs = std::filesystem;

void writeToTextFile(std::string& fileName,std::vector<std::pair<Tuple, RID>>& studentRIDS)
{
    std::ofstream outFile(fileName, std::ios::trunc);
    if(!outFile.is_open()) throw std::runtime_error("Failed to open file");


    for(const auto& [tuple, rid]: studentRIDS)
    {
        outFile << rid.pageID.tableID << " " << rid.pageID.pageNumber << " " << rid.tupleSlot << "\n";
    }

    if(!outFile) throw std::runtime_error("Failed to write to file");
    outFile.close();
}

void readFromTextFile(std::string& fileName, std::vector<RID>& rids)
{
    std::ifstream inFile(fileName);
    if(!inFile.is_open()) throw std::runtime_error("Failed to open file");

    std::vector<RID> readRIDs;

    int64_t tableID;
    int64_t pageNumber;
    int16_t tupleSlot;

    while(inFile >> tableID >> pageNumber >> tupleSlot)
    {
        readRIDs.push_back(RID{PageID{tableID, pageNumber}, tupleSlot});
    }

    if(!inFile.eof()) throw std::runtime_error("Invalid RID data in file");

    rids = std::move(readRIDs);
}

bool compareRID(RID& r1, RID& r2)
{
    if(r1.pageID.tableID != r2.pageID.tableID || r1.pageID.pageNumber != r2.pageID.pageNumber || r1.tupleSlot != r2.tupleSlot) return false;
    return true;
}

std::filesystem::path getExecutablePath()
{
    std::wstring buffer(32768, L'\0');

    DWORD length = GetModuleFileNameW(
        nullptr,
        buffer.data(),
        static_cast<DWORD>(buffer.size())
    );

    if(length == 0 || length >= buffer.size())
        throw std::runtime_error("Failed to retrieve executable path");

    buffer.resize(length);

    return std::filesystem::path(buffer);
}

bool step_one(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS);

bool step_two(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS);

bool step_three(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS);

bool step_four(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS);

bool step_five(std::shared_ptr<Database>database, std::vector<std::pair<Tuple, RID>>& studentRIDS);

bool step_six(std::shared_ptr<Database>database, std::vector<std::pair<Tuple, RID>>& studentRIDS);

bool step_seven();

Schema studentSchema({
    Field{"id", FieldType::INT64},
    Field{"name", FieldType::STRING32},
    Field{"dept", FieldType::STRING32},
    Field{"score", FieldType::INT64}
});

Schema departmentsSchema({
    Field{"dept", FieldType::STRING32},
    Field{"building", FieldType::STRING32}
});

int main(int argc, char* argv[])
{
    
    if(argc == 1)
    {
        std::filesystem::path directoryPath = directoryString;
        std::filesystem::remove_all(directoryString);
        std::shared_ptr<Database> database = std::make_shared<Database>(directoryPath);

        std::vector<std::pair<Tuple, RID>> originalStudentRIDs;
        std::cout << "\t---Step 1 Test---";
        try{
            std::cout << "\t" << ((step_one(database, originalStudentRIDs)) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }

        std::cout << "\t---Step 2 Test---";
        try{
            std::cout << "\t" << ((step_two(database, originalStudentRIDs)) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }

        std::cout << "\t---Step 3 Test---";
        try{
            std::cout << "\t" << ((step_three(database, originalStudentRIDs)) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }

        std::cout << "\t---Step 4 Test---";
        try{
            std::cout << "\t" << ((step_four(database, originalStudentRIDs)) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }

        std::cout << "\t---Step 5 Test---";
        try{
            std::cout << "\t" << ((step_five(database, originalStudentRIDs)) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }

        std::cout << "\t---Step 6 Test---";
        try{
            std::cout << "\t" << ((step_six(database, originalStudentRIDs)) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }

        database->flushAllPages();
        
        std::vector<std::pair<Tuple,RID>> currentStudentResults = database->scan("students");
        // Write the RIDS in std::vector<std::pair<Tuple, RID>> studentRIDs to a textfile: "RIDS.txt"
        writeToTextFile(fileString , currentStudentResults);

        std::cout << "\n\tStarting Process for Step 7\n";
        std::filesystem::path exePath = getExecutablePath();

        std::ostringstream command;

        command << "\"" << exePath.string() << "\" "
                << "blah";
        
        int retVal = std::system(command.str().c_str());
        return retVal;
    }
    else if(argc == 2)
    {
        std::cout << "\t---Step 7 Test---";
        try{
            std::cout << "\t" << ((step_seven()) ? "[Pass" : "[Fail") << "]\n";
        }
        catch(std::exception& e)
        {
            std::cout << "[Fail]: " << e.what();
            return -1;
        }
        return 0;
    }

    return 0;
    
}

/*
    Create Tables w/ Insert
    students(id INT64, name STRING32, dept STRING32, score INT64) 

    (1, "Alice", "CS",   90) 

    (2, "Bob",   "CS",   75) 

    (3, "Carol", "Math", 85) 

    (4, "Dan",   "Math", 75) 

    (5, "Eve",   "CS",   90) 
    Return Count = 5

    

    departments(dept STRING32, building STRING32) 

    ("CS",   "Tech") 

    ("Math", "Science") 

    ("Art",  "Studio") 
    Return Count = 3

    Sequential Scan must return all 5 students & all 3 departments
    Retain student RIDs
*/
bool step_one(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDs)
{
    std::vector<Row> studentRows = 
    {
        Row{Tuple({int64_t{1}, std::string{"Alice"}, std::string{"CS"}, int64_t{90}})},
        Row{Tuple({int64_t{2}, std::string{"Bob"}, std::string{"CS"}, int64_t{75}})},
        Row{Tuple({int64_t{3}, std::string{"Carol"}, std::string{"Math"}, int64_t{85}})},
        Row{Tuple({int64_t{4}, std::string{"Dan"}, std::string{"Math"}, int64_t{75}})},
        Row{Tuple({int64_t{5}, std::string{"Eve"}, std::string{"CS"}, int64_t{90}})}
    };

    std::vector<Row> deptRows = 
    {
        Row{Tuple({std::string{"CS"}, std::string{"Tech"}})},
        Row{Tuple({std::string{"Math"}, std::string{"Science"}})},
        Row{Tuple({std::string{"Art"}, std::string{"Studio"}})}
    };

    std::shared_ptr<const std::vector<Row>> studentRowsPointer = std::make_shared<const std::vector<Row>>(std::move(studentRows));
    std::shared_ptr<const std::vector<Row>> deptRowsPointer = std::make_shared<const std::vector<Row>>(std::move(deptRows));

    database->createTable("students", studentSchema);
    database->createTable("depts", departmentsSchema);

    std::unique_ptr<Executor> studentValues = std::make_unique<ValuesExecutor>(studentRowsPointer, std::make_shared<const QuerySchema>(studentSchema));
    std::unique_ptr<Executor> deptValues = std::make_unique<ValuesExecutor>(deptRowsPointer, std::make_shared<const QuerySchema>(departmentsSchema));

    InsertExecutor studentInsert(database, "students", std::move(studentValues));
    InsertExecutor deptInsert(database, "depts", std::move(deptValues));

    studentInsert.Init();
    deptInsert.Init();

    Row result;

    // Insert Students
    studentInsert.Next(result);

    if(std::get<int64_t>(result.tuple.getTupleValueIndex(0)) != 5) return false;

    std::vector<std::pair<Tuple, RID>> scanResult = database->scan("students");
    if(scanResult.size() != 5) return false;
    studentRIDs = scanResult;

    // Insert Departments
    deptInsert.Next(result);
    if(std::get<int64_t>(result.tuple.getTupleValueIndex(0)) != 3) return false;

    scanResult = database->scan("depts");
    if(scanResult.size() != 3) return false;

    return true;
}

/*
    Filter & Project

    Execute the following:
    SELECT name, score + 5 AS adjusted_score 
    FROM students WHERE dept = 'CS' AND score >= 90; 

    Expect:  ("Alice", 95) and ("Eve", 95)
    Check filtering with score > 100 returns no rows
*/
bool step_two(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS)
{
    // Get rows
    std::unique_ptr<Executor> scan = std::make_unique<SequentialScanExecutor>(database, "students");

    // Make filter
    std::unique_ptr<Expression> filterExpression = 
        std::make_unique<AndExpression>(
            std::make_unique<GreaterThanEqualExpression>(
                std::make_unique<ColumnExpression>(3),
                std::make_unique<ConstantExpression>(int64_t{90})
            ),
            std::make_unique<EqualExpression>(
                std::make_unique<ColumnExpression>(2),
                std::make_unique<ConstantExpression>(std::string{"CS"})
            )
    );
    
    // Use Filter
    std::unique_ptr<Executor> filter = std::make_unique<FilterExecutor>(std::move(scan), std::move(filterExpression));

    // Select
    std::vector<ProjectionColumn> projectionList;

    projectionList.push_back(ProjectionColumn{Field{"name", FieldType::STRING32}, std::make_unique<ColumnExpression>(1)});

    projectionList.push_back(ProjectionColumn{Field{"adjusted_score", FieldType::INT64}, 
        std::make_unique<AdditionExpression>(
            std::make_unique<ColumnExpression>(3), 
            std::make_unique<ConstantExpression>(int64_t{5})
        )} // Score + 5, 
    );

    ProjectionExecutor projection(std::move(filter), std::move(projectionList));

    projection.Init();

    Row row;

    std::vector<Row> projectionRows;

    while(projection.Next(row))
    {
        projectionRows.push_back(row);
    }
    projection.Close();

    // Check Results
    if(projectionRows.size() != 2) return false;

    if(std::get<std::string>(projectionRows[0].tuple.getTupleValueIndex(0)) != "Alice") return false;
    if(std::get<int64_t>(projectionRows[0].tuple.getTupleValueIndex(1)) != int64_t{95}) return false;

    if(std::get<std::string>(projectionRows[1].tuple.getTupleValueIndex(0)) != "Eve") return false;
    if(std::get<int64_t>(projectionRows[1].tuple.getTupleValueIndex(1)) != int64_t{95}) return false;

    std::cout << "\n\tResults:\n";
    for(const Row& row : projectionRows)
    {
        const auto& values = row.tuple.getTupleValues();

        std::cout << "\t"
        << std::get<std::string>(values[0])
        << ", " << std::get<int64_t>(values[1]) << "\n";
    }

    // Check filtering with score > 100 returns no results
    
    std::unique_ptr<Executor> scan2 = std::make_unique<SequentialScanExecutor>(database, "students");

    std::unique_ptr<Expression> filterExpression2 = 
        std::make_unique<GreaterThanExpression>(
            std::make_unique<ColumnExpression>(3),
            std::make_unique<ConstantExpression>(int64_t{100})
        );

    std::unique_ptr<Executor> filter2 = std::make_unique<FilterExecutor>(std::move(scan2), std::move(filterExpression2));

    std::vector<ProjectionColumn> projectionList2;

    projectionList2.push_back(ProjectionColumn{Field{"name", FieldType::STRING32}, std::make_unique<ColumnExpression>(1)});

    projectionList2.push_back(ProjectionColumn{Field{"adjusted_score", FieldType::INT64}, 
        std::make_unique<AdditionExpression>(
            std::make_unique<ColumnExpression>(3), 
            std::make_unique<ConstantExpression>(int64_t{5})
        )} // Score + 5, 
    );

    ProjectionExecutor projection2(std::move(filter2), std::move(projectionList2));
    projection2.Init();

    Row row2;
    std::vector<Row> projectionRows2;
    
    while(projection2.Next(row))
    {
        projectionRows2.push_back(row);
    }
    projection2.Close();

    if(!projectionRows2.empty()) return false;

    return true;
}

/*
    Join

    Join students to departments on equal dept values
    Project name & building
    Expect:  ("Alice", "Tech"), ("Bob", "Tech"), ("Carol", "Science"), ("Dan", "Science"), and ("Eve", "Tech")
    Art contributes no rows!
*/
bool step_three(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS)
{
    std::unique_ptr<Executor> studentScan = std::make_unique<SequentialScanExecutor>(database, "students");
    std::unique_ptr<Executor> deptScan = std::make_unique<SequentialScanExecutor>(database, "depts");

    std::unique_ptr<Expression> filterExpression = 
        std::make_unique<EqualExpression>(
            std::make_unique<ColumnExpression>(2),
            std::make_unique<ColumnExpression>(4)
    );

    std::unique_ptr<Executor> join = std::make_unique<NestedLoopExecutor>(
        std::move(studentScan), std::move(deptScan), std::move(filterExpression)
    );

    std::vector<ProjectionColumn> projectionList;
    projectionList.push_back(ProjectionColumn{Field{"name", FieldType::STRING32}, std::make_unique<ColumnExpression>(1)});
    projectionList.push_back(ProjectionColumn{Field{"building", FieldType::STRING32}, std::make_unique<ColumnExpression>(5)});

    ProjectionExecutor projection(std::move(join), std::move(projectionList));
    projection.Init();
    Row row;

    std::vector<Row> projectionRows;

    while(projection.Next(row))
    {
        projectionRows.push_back(row);
    }
    projection.Close();

    if(projectionRows.size() != 5) return false;

    if(std::get<std::string>(projectionRows[0].tuple.getTupleValueIndex(0)) != "Alice") return false;
    if(std::get<std::string>(projectionRows[0].tuple.getTupleValueIndex(1)) != "Tech") return false;

    if(std::get<std::string>(projectionRows[1].tuple.getTupleValueIndex(0)) != "Bob") return false;
    if(std::get<std::string>(projectionRows[1].tuple.getTupleValueIndex(1)) != "Tech") return false;

    if(std::get<std::string>(projectionRows[2].tuple.getTupleValueIndex(0)) != "Carol") return false;
    if(std::get<std::string>(projectionRows[2].tuple.getTupleValueIndex(1)) != "Science") return false;

    if(std::get<std::string>(projectionRows[3].tuple.getTupleValueIndex(0)) != "Dan") return false;
    if(std::get<std::string>(projectionRows[3].tuple.getTupleValueIndex(1)) != "Science") return false;

    if(std::get<std::string>(projectionRows[4].tuple.getTupleValueIndex(0)) != "Eve") return false;
    if(std::get<std::string>(projectionRows[4].tuple.getTupleValueIndex(1)) != "Tech") return false;
    
    std::cout << "\n\tResults:\n";
    for(const Row& row : projectionRows)
    {
        const auto& values = row.tuple.getTupleValues();

        std::cout << "\t"
        << std::get<std::string>(values[0])
        << ", " << std::get<std::string>(values[1]) << "\n";
    }
    return true;
}

/*
    Aggregate
    Group stuends by dept & compute... COUNT(*), SUM(score), MIN(score), MAX(score)
    Expect:
    ("CS", 3, 255, 75, 90) and ("Math", 2, 160, 75, 85)
    Global Aggregation must return:
     (5, 415, 75, 90)
*/
bool step_four(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS)
{
    std::unique_ptr<Executor> studentScan = std::make_unique<SequentialScanExecutor>(database, "students");

    std::vector<GroupByExpression> groupByExpressions;
    groupByExpressions.push_back({std::make_unique<ColumnExpression>(2), Field{"dept", FieldType::STRING32}});

    std::vector<AggregationExpression> aggregates;
    aggregates.push_back({Field{"count", FieldType::INT64}, AggregationType::COUNT, nullptr});
    aggregates.push_back({Field{"score_sum", FieldType::INT64}, AggregationType::SUM, std::make_unique<ColumnExpression>(3)});
    aggregates.push_back({Field{"score_min", FieldType::INT64}, AggregationType::MIN, std::make_unique<ColumnExpression>(3)});
    aggregates.push_back({Field{"score_max", FieldType::INT64}, AggregationType::MAX, std::make_unique<ColumnExpression>(3)});

    AggregationExecutor aggregator(std::move(studentScan), std::move(groupByExpressions), std::move(aggregates));
    aggregator.Init();
    Row row;
    std::vector<Row> aggregationRows;

    while(aggregator.Next(row))
    {
        aggregationRows.push_back(row);
    }
    
    // Check results
    if(aggregationRows.size() != 2) return false;
    
    if(std::get<std::string>(aggregationRows[0].tuple.getTupleValueIndex(0)) != "CS") return false;
    if(std::get<int64_t>(aggregationRows[0].tuple.getTupleValueIndex(1)) != 3) return false;
    if(std::get<int64_t>(aggregationRows[0].tuple.getTupleValueIndex(2)) != 255) return false;
    if(std::get<int64_t>(aggregationRows[0].tuple.getTupleValueIndex(3)) != 75) return false;
    if(std::get<int64_t>(aggregationRows[0].tuple.getTupleValueIndex(4)) != 90) return false;

    if(std::get<std::string>(aggregationRows[1].tuple.getTupleValueIndex(0)) != "Math") return false;
    if(std::get<int64_t>(aggregationRows[1].tuple.getTupleValueIndex(1)) != 2) return false;
    if(std::get<int64_t>(aggregationRows[1].tuple.getTupleValueIndex(2)) != 160) return false;
    if(std::get<int64_t>(aggregationRows[1].tuple.getTupleValueIndex(3)) != 75) return false;
    if(std::get<int64_t>(aggregationRows[1].tuple.getTupleValueIndex(4)) != 85) return false;

    std::cout << "\n\tResults:\n";
    for(const Row& row : aggregationRows)
    {
        const auto& values = row.tuple.getTupleValues();
        std::cout << "\t"
        << std::get<std::string>(values[0])
        << ", " << std::get<int64_t>(values[2])
        << ", " << std::get<int64_t>(values[3])
        << ", " << std::get<int64_t>(values[4]) << "\n";
    }

    return true;

}

/*
    Sort and Limit
    Sort students by
        Score : Desc, ID : Asc
    Limit by 3 Rows
    Project: ID, name, score
    Expect:  (1, "Alice", 90), (5, "Eve", 90), (3, "Carol", 85)

    Seperate Limit test for 0 should return 0 rows w/out pulling from child
*/
bool step_five(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS)
{
    std::unique_ptr<Executor> studentScan = std::make_unique<SequentialScanExecutor>(database, "students");

    std::vector<SortKey> sortKeys;
    sortKeys.push_back({std::make_unique<ColumnExpression>(3), SortDirection::DESC});
    sortKeys.push_back({std::make_unique<ColumnExpression>(1), SortDirection::ASC});

    std::unique_ptr<Executor> sortExecutor = std::make_unique<SortExecutor>(std::move(studentScan), std::move(sortKeys));

    std::unique_ptr<Executor> limitExecutor = std::make_unique<LimitExecutor>(std::move(sortExecutor), 3);

    std::vector<ProjectionColumn> projectionList;

    projectionList.push_back(ProjectionColumn{Field{"id", FieldType::INT64}, std::make_unique<ColumnExpression>(0)});

    projectionList.push_back(ProjectionColumn{Field{"name", FieldType::STRING32}, std::make_unique<ColumnExpression>(1)});

    projectionList.push_back(ProjectionColumn{Field{"score", FieldType::INT64}, std::make_unique<ColumnExpression>(3)}); 

    ProjectionExecutor projection(std::move(limitExecutor), std::move(projectionList));

    projection.Init();

    Row row;

    std::vector<Row> projectionRows;

    while(projection.Next(row))
    {
        projectionRows.push_back(row);
    }

    projection.Close();

    // Check Results
    if(projectionRows.size() != 3) return false;

    if(std::get<int64_t>(projectionRows[0].tuple.getTupleValueIndex(0)) != 1) return false;
    if(std::get<std::string>(projectionRows[0].tuple.getTupleValueIndex(1)) != "Alice") return false;
    if(std::get<int64_t>(projectionRows[0].tuple.getTupleValueIndex(2)) != 90) return false;

    if(std::get<int64_t>(projectionRows[1].tuple.getTupleValueIndex(0)) != 5) return false;
    if(std::get<std::string>(projectionRows[1].tuple.getTupleValueIndex(1)) != "Eve") return false;
    if(std::get<int64_t>(projectionRows[1].tuple.getTupleValueIndex(2)) != 90) return false;

    if(std::get<int64_t>(projectionRows[2].tuple.getTupleValueIndex(0)) != 3) return false;
    if(std::get<std::string>(projectionRows[2].tuple.getTupleValueIndex(1)) != "Carol") return false;
    if(std::get<int64_t>(projectionRows[2].tuple.getTupleValueIndex(2)) != 85) return false;

    std::cout << "\n\tResults:\n";
    for(const Row& row : projectionRows)
    {
        const auto& values = row.tuple.getTupleValues();

        std::cout << "\t"
        << std::get<int64_t>(values[0])
        << ", " << std::get<std::string>(values[1]) 
        << ", " << std::get<int64_t>(values[2]) << "\n";
    }


    std::unique_ptr<Executor> studentScan2 = std::make_unique<SequentialScanExecutor>(database, "students");

    std::vector<SortKey> sortKeys2;
    sortKeys.push_back({std::make_unique<ColumnExpression>(3), SortDirection::DESC});
    sortKeys.push_back({std::make_unique<ColumnExpression>(1), SortDirection::ASC});

    std::unique_ptr<Executor> sortExecutor2 = std::make_unique<SortExecutor>(std::move(studentScan2), std::move(sortKeys2));

    std::unique_ptr<Executor> limitExecutor2 = std::make_unique<LimitExecutor>(std::move(sortExecutor2), 0);

    std::vector<ProjectionColumn> projectionList2;

    projectionList2.push_back(ProjectionColumn{Field{"id", FieldType::INT64}, std::make_unique<ColumnExpression>(0)});

    projectionList2.push_back(ProjectionColumn{Field{"name", FieldType::STRING32}, std::make_unique<ColumnExpression>(1)});

    projectionList2.push_back(ProjectionColumn{Field{"score", FieldType::INT64}, std::make_unique<ColumnExpression>(3)}); 

    ProjectionExecutor projection2(std::move(limitExecutor2), std::move(projectionList2));

    projection2.Init();

    std::vector<Row> projection2Rows;

    while(projection2.Next(row))
    {
        projection2Rows.push_back(row);
    }

    projection2.Close();

    // Check Results
    if(projection2Rows.size() != 0) return false;

    return true;
}

/*
    Update & Delete through plans
    Use update over Filter/Scan plan to...
        Set score = score + 10 Where ID = 2
        Expect:
            Affected_Rows = 1
            Bob's Score = 85
            AND same RID
        
    Use delete over Filter/Scan plan to...
        Where ID = 4, delete
        Expect:
            Affected_rows = 1
            Dan absent from scans
            Read of RID reports not found
    
    Additional Next calls on both executors return false & perf no extra writes
*/
bool step_six(std::shared_ptr<Database> database, std::vector<std::pair<Tuple, RID>>& studentRIDS)
{
    std::unique_ptr<Executor> studentScan = std::make_unique<SequentialScanExecutor>(database, "students");

    std::unique_ptr<Expression> filterExpression = 
        std::make_unique<EqualExpression>(
            std::make_unique<ColumnExpression>(0),
            std::make_unique<ConstantExpression>(int64_t{2})
    );

    std::unique_ptr<Executor> filterExecutor = std:: make_unique<FilterExecutor>(std::move(studentScan), std::move(filterExpression));

    std::vector<UpdateExpression> updateExpressions;
    updateExpressions.push_back({3, {std::make_unique<AdditionExpression>(std::make_unique<ColumnExpression>(3), std::make_unique<ConstantExpression>(10))}});

    UpdateExecutor updateExecutor(database, "students", std::move(filterExecutor), std::move(updateExpressions));

    updateExecutor.Init();
    Row row;
    while(updateExecutor.Next(row));
    updateExecutor.Close();

    if(std::get<int64_t>(row.tuple.getTupleValueIndex(0)) != 1) return false;

    std::cout << "\n\tResults: \n"; 
    std::cout << "\tUpdate Affected Rows: " << std::get<int64_t>(row.tuple.getTupleValueIndex(0)) << "\n";

    Tuple bobRead = database->readTuple("students", studentRIDS[1].second);

    if(std::get<int64_t>(bobRead.getTupleValueIndex(3)) != 85) return false;

    std::cout << "\tBob's Score: " << std::get<int64_t>(bobRead.getTupleValueIndex(3)) << "\n";

    std::unique_ptr<Executor> studentScan2 = std::make_unique<SequentialScanExecutor>(database, "students");

    std::unique_ptr<Expression> filterExpression2 = 
        std::make_unique<EqualExpression>(
            std::make_unique<ColumnExpression>(0),
            std::make_unique<ConstantExpression>(int64_t{4})
    );

    std::unique_ptr<Executor> filterExecutor2 = std:: make_unique<FilterExecutor>(std::move(studentScan2), std::move(filterExpression2));

    DeleteExecutor deleteExecutor(database, "students", std::move(filterExecutor2));

    deleteExecutor.Init();
    while(deleteExecutor.Next(row));
    deleteExecutor.Close();

    if(std::get<int64_t>(row.tuple.getTupleValueIndex(0)) != 1) return false;

    std::cout << "\n\tResults: \n"; 
    std::cout << "\tDelete Affected Rows: " << std::get<int64_t>(row.tuple.getTupleValueIndex(0)) << "\n";

    Tuple danRead = database->readTuple("students", studentRIDS[3].second);

    if(!danRead.getTupleValues().empty()) return false;

    SequentialScanExecutor studentScan3(database, "students");

    std::vector<Row> sequentialScanRows;

    studentScan3.Init();

    while(studentScan3.Next(row))
    {
        sequentialScanRows.push_back(row);
    }

    studentScan3.Close();

    if(sequentialScanRows.size() != 4) return false;

    std::cout << "\tScan Results: \n";
    for(const Row& row : sequentialScanRows)
    {
        const auto& values = row.tuple.getTupleValues();

        std::cout << "\t"
        << std::get<int64_t>(values[0])
        << ", " << std::get<std::string>(values[1]) 
        << ", " << std::get<std::string>(values[2])
        << ", " << std::get<int64_t>(values[3]) << "\n";
    }
    return true;
}

/*
    Persist reopen & query again
    Checkpoint & close the database
    Start seperate verification process that opens the same files w/out create/reinserting data
    Must find:
        Students w/ IDs: 1, 2, 3, 5
        Bob's score = 85, Dan Absent
        Grouped COUNT & SUM(score) must return:  ("CS", 3, 265) and ("Math", 1, 85)
        Global Sum = 350
        Save RIDS in small test-metadata file if needed for verification
*/
bool step_seven()
{
    std::filesystem::path directoryPath = directoryString;
    std::shared_ptr<Database> database = std::make_shared<Database>(directoryPath);
    std::vector<RID> studentRIDs;

    readFromTextFile(fileString, studentRIDs);
    if(studentRIDs.size() != 4) return false;

    std::vector<std::pair<Tuple, RID>> scanResult = database->scan("students");
    if(scanResult.size() != 4) return false;

    std::cout << "\n\tScan Results: \n";
    for(const std::pair<Tuple,RID>& result : scanResult)
    {
        const auto& values = result.first.getTupleValues();

        if(std::get<int64_t>(values[0]) == 4) return false; // check for Dan
        if(std::get<int64_t>(values[0]) == 2 && std::get<int64_t>(values[3]) != 85) return false; // check for bob's score
        
        std::cout << "\t"
        << std::get<int64_t>(values[0])
        << ", " << std::get<std::string>(values[1]) 
        << ", " << std::get<std::string>(values[2])
        << ", " << std::get<int64_t>(values[3]) << "\n";
    }

    // Make sure same RIDS
    if(!compareRID(scanResult[0].second, studentRIDs[0])) return false;
    if(!compareRID(scanResult[1].second, studentRIDs[1])) return false;
    if(!compareRID(scanResult[2].second, studentRIDs[2])) return false;
    if(!compareRID(scanResult[3].second, studentRIDs[3])) return false;


    std::unique_ptr<Executor> studentScan = std::make_unique<SequentialScanExecutor>(database, "students");

    std::vector<GroupByExpression> groupByExpressions;
    groupByExpressions.push_back({std::make_unique<ColumnExpression>(2), Field{"dept", FieldType::STRING32}});

    std::vector<AggregationExpression> aggregates;
    aggregates.push_back({Field{"count", FieldType::INT64}, AggregationType::COUNT, nullptr});
    aggregates.push_back({Field{"score_sum", FieldType::INT64}, AggregationType::SUM, std::make_unique<ColumnExpression>(3)});

    AggregationExecutor aggregator(std::move(studentScan), std::move(groupByExpressions), std::move(aggregates));
    aggregator.Init();
    Row row;
    std::vector<Row> aggregationRows;

    while(aggregator.Next(row))
    {
        aggregationRows.push_back(row);
    }
    aggregator.Close();
    
    // Check results
    if(aggregationRows.size() != 2) return false;
    
    if(std::get<std::string>(aggregationRows[0].tuple.getTupleValueIndex(0)) != "CS") return false;
    if(std::get<int64_t>(aggregationRows[0].tuple.getTupleValueIndex(1)) != 3) return false;
    if(std::get<int64_t>(aggregationRows[0].tuple.getTupleValueIndex(2)) != 265) return false;

    if(std::get<std::string>(aggregationRows[1].tuple.getTupleValueIndex(0)) != "Math") return false;
    if(std::get<int64_t>(aggregationRows[1].tuple.getTupleValueIndex(1)) != 1) return false;
    if(std::get<int64_t>(aggregationRows[1].tuple.getTupleValueIndex(2)) != 85) return false;

    std::cout << "\n\tAggregation Results:\n";
    for(const Row& row : aggregationRows)
    {
        const auto& values = row.tuple.getTupleValues();
        std::cout << "\t"
        << std::get<std::string>(values[0])
        << ", " << std::get<int64_t>(values[1])
        << ", " << std::get<int64_t>(values[2]) << "\n";
    }

    std::unique_ptr<Executor> studentScan2 = std::make_unique<SequentialScanExecutor>(database, "students");

    std::vector<GroupByExpression> groupByExpressions2;

    std::vector<AggregationExpression> aggregates2;
    aggregates2.push_back({Field{"global_sum", FieldType::INT64}, AggregationType::SUM, std::make_unique<ColumnExpression>(3)});

    AggregationExecutor aggregator2(std::move(studentScan2), std::move(groupByExpressions2), std::move(aggregates2));
    aggregator2.Init();
    std::vector<Row> aggregationRows2;

    while(aggregator2.Next(row))
    {
        aggregationRows2.push_back(row);
    }
    aggregator2.Close();
    
    // Check results
    if(aggregationRows2.size() != 1) return false;
    
    if(std::get<int64_t>(aggregationRows2[0].tuple.getTupleValueIndex(0)) != 350) return false;

    std::cout << "\n\tGlobal Aggregation Sum Results:\n";
    for(const Row& row : aggregationRows2)
    {
        const auto& values = row.tuple.getTupleValues();
        std::cout << "\t"
        << std::get<int64_t>(values[0]) << "\n";
    }

    return true;
}
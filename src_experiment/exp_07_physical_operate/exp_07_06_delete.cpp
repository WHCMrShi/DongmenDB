//
// Created by sam on 2018/9/18.
// Completed by Shils Shek on 2019/9/10.
//
#include <physicalplan/ExecutionPlan.h>

/*执行delete语句的物理计划，返回删除的记录条数
 * 返回大于等于0的值，表示删除的记录条数；
 * 返回小于0的值，表示删除过程中出现错误。
 * */

int ExecutionPlan::executeDelete(DongmenDB *db, sql_stmt_delete *sqlStmtDelete, Transaction *tx){
    size_t deleted_count = 0;
    Scan* scan = generateScan(db, sqlStmtDelete->where, tx);
    scan->beforeFirst();

    while(scan->next()) {
        scan->deleteRecord();
        deleted_count++;
    }
    scan->close();
    return deleted_count;
};

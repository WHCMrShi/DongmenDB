//
// Created by sam on 2018/9/18.
// Completed by Shils Shek on 2019/9/10.
//
#include <physicalplan/ExecutionPlan.h>
#include <physicalplan/Scan.h>

int ExecutionPlan::executeUpdate(DongmenDB *db, sql_stmt_update *sqlStmtUpdate, Transaction *tx){

    size_t updated_count = 0;
    Scan* scan = generateScan(db, sqlStmtUpdate->where, tx);
    scan->beforeFirst();

    while(scan->next()) {
        for (size_t i = 0; i < sqlStmtUpdate->fieldsExpr.size(); i++) {
            char *currentFieldName = sqlStmtUpdate->fields.at(i);
            variant *val = (variant *) calloc(sizeof(variant), 1);
            int field_type = scan->getField(sqlStmtUpdate->tableName, currentFieldName)->type;

            scan->evaluateExpression(sqlStmtUpdate->fieldsExpr.at(i), scan, val);

            if (val->type != field_type) {
                fprintf(stderr, "field type error\n");
                return DONGMENDB_EINVALIDSQL;
            }

            if (val->type == DATA_TYPE_INT) {
                scan->setInt(sqlStmtUpdate->tableName, currentFieldName, val->intValue);
            } else if (val->type == DATA_TYPE_CHAR) {
                scan->setString(sqlStmtUpdate->tableName, currentFieldName, val->strValue);
            }
        }
        updated_count++;
    }
    scan->close();
    return updated_count;
};

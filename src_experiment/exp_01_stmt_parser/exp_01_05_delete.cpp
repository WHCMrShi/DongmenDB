//
// Created by Sam on 2018/2/13.
// Completed by Shils Shek on 2019/9/10.
//

#include <dongmensql/sqlstatement.h>
#include <parser/StatementParser.h>

/**
 * 在现有实现基础上，实现delete from子句
 *
 *  支持的delete语法：
 *
 *  DELETE FROM <table_nbame>
 *  WHERE <logical_expr>
 *
 * 解析获得 sql_stmt_delete 结构
 */

sql_stmt_delete *DeleteParser::parse_sql_stmt_delete(){
    char *tableName = NULL;
    SRA_t *where;
    sql_stmt_delete *sqlStmtDelete = (sql_stmt_delete *)calloc(sizeof(sql_stmt_delete),1);

    Token *token = this->parseNextToken();
    if(!this->matchToken(TOKEN_RESERVED_WORD, "delete")){
        strcpy(this->parserMessage, "grammatical error: missing delete.");
        return NULL;
    }

    token = this->parseNextToken();
    if (token->type == TOKEN_WORD) {
        tableName = new_id_name();
        strcpy(tableName, token->text);
    } else {
        strcpy(this->parserMessage, "invalid sql: missing table name.");
        return NULL;
    }
    TableReference_t *ref = TableReference_make(tableName, NULL);

    token = this->parseNextToken();
    if(token == NULL || token->type == TOKEN_SEMICOLON){
        /*无where，update语句结束，对各字段赋值，并返回*/
        where = SRATable(ref);
        sqlStmtDelete->tableName = tableName;
        sqlStmtDelete->where = where;
        return sqlStmtDelete;
    }


    //token = this->parseNextToken();
    token = this->parseEatAndNextToken();
    if(!this->matchToken(TOKEN_RESERVED_WORD, "where")){
        strcpy(this->parserMessage, "grammatical error: missing where.");
        return NULL;
    }

    /*解析where子句中的条件表达式*/
    token = this->parseNextToken();
    Expression *whereExpr = this->parseExpressionRD();
    if (this->parserStateType == PARSER_WRONG) {
        return NULL;
    }
    where = SRATable(ref);
    where = SRASelect(where, whereExpr);
    token = this->parseNextToken();

    if (token == NULL || token->type == TOKEN_SEMICOLON) {
        sqlStmtDelete->tableName = tableName;
        sqlStmtDelete->where = where;
        /*
        printf("\nToken Recognized: \ntableName: %s\n",tableName);
        printf("where: ");
        SRA_print(where);
        putchar('\n');
        */
        return sqlStmtDelete;
    }
};
//
// Created by Sam on 2018/2/13.
// Completed by Shils Shek on 2019/9/10.
//

#include <dongmensql/sqlstatement.h>
#include <parser/StatementParser.h>

/**
 * 在现有实现基础上，实现update from子句
 *
 * 支持的update语法：
 *
 * UPDATE <table_name> SET <field1> = <expr1>[, <field2 = <expr2>, ..]
 * WHERE <logical_expr>
 *
 * 解析获得 sql_stmt_update 结构
 */

sql_stmt_update *UpdateParser::parse_sql_stmt_update() {
    char *tableName = NULL;
    vector<char*> fields ;
    vector<Expression*> fieldsExpr ;
    SRA_t *where ;
    sql_stmt_update *sqlStmtUpdate = (sql_stmt_update *)calloc(sizeof(sql_stmt_update),1);

    Token *token = this->parseNextToken();
    if(!this->matchToken(TOKEN_RESERVED_WORD, "update")){
        strcpy(this->parserMessage, "grammatical error: missing update.");
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
    where = SRATable(ref);

    token = this->parseEatAndNextToken();
    if(!this->matchToken(TOKEN_RESERVED_WORD, "set")){
        strcpy(this->parserMessage, "grammatical error: missing set.");
        return NULL;
    }

    /*解析字段名*/
    token = this->parseNextToken();
    if (token->type == TOKEN_WORD) {
        while (token->type == TOKEN_WORD) {
            char *fieldName = new_id_name();
            strcpy(fieldName, token->text);
            fields.push_back(fieldName);
            printf("%s",fieldName);

            token = this->parseEatAndNextToken();
            if(token->type == TOKEN_EQ){
                /*解析值或表达式*/
                token = this->parseEatAndNextToken();
                Expression *fExpr0 = this->parseExpressionRD();
                fieldsExpr.push_back(fExpr0);

                while(token != NULL && token->type != TOKEN_SEMICOLON && token->type != TOKEN_COMMA && token->type != TOKEN_RESERVED_WORD)
                    token = this->parseNextToken();
                if(token == NULL || token->type == TOKEN_SEMICOLON){
                    /*无where，update语句结束，对各字段赋值，并返回*/
                    sqlStmtUpdate->tableName = tableName;
                    sqlStmtUpdate->fields = fields;
                    sqlStmtUpdate->fieldsExpr = fieldsExpr;
                    sqlStmtUpdate->where = where;
                    return sqlStmtUpdate;
                }
                else if(token->type == TOKEN_COMMA)
                    token = this->parseEatAndNextToken();
                else
                    break;
            }
        }
    } else {
        strcpy(this->parserMessage, "invalid sql: missing field name.");
        return NULL;
    }

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
    where = SRASelect(where, whereExpr);
    token = this->parseNextToken();

    /*update语句结束，对各字段赋值，并返回*/
    if (token == NULL || token->type == TOKEN_SEMICOLON) {
        sqlStmtUpdate->tableName = tableName;
        sqlStmtUpdate->fields = fields;
        sqlStmtUpdate->fieldsExpr = fieldsExpr;
        sqlStmtUpdate->where = where;
/*
 * 检验解析结果
        printf("\nToken Recognized: \ntableName: %s\n",tableName);
        for(int i=0;i<fields.size();i++)
            printf("fields %d: %s\n",i+1,fields.at(i));
        for(int i=0;i<fieldsExpr.size();i++) {
            printf("fieldsExpr %d: ",i+1);
            fieldsExpr.at(i)->expression_print(fieldsExpr.at(i), NULL);
            putchar('\n');
        }
        printf("\nwhere: ");
        SRA_print(where);
        putchar('\n');
*/
        return sqlStmtUpdate;
    }
};

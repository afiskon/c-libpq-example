#include <stdio.h>
#include <stdlib.h>
#include <postgresql/libpq-fe.h>

#define UNUSED(x) (void)(x)

static const char* user_phone_arr[][2] = {
    { "user111", "phone111" },
    { "user222", "phone222" },
    { "user333", "phone333" },
    { NULL, NULL }
};

static PGconn* conn = NULL;
static PGresult* res = NULL;

static void
terminate(int code)
{
    if(code != 0)
        fprintf(stderr, "%s\n", PQerrorMessage(conn));

    if(res != NULL)
        PQclear(res);

    if(conn != NULL)
        PQfinish(conn);

    exit(code);
}

static void
clearRes()
{
    PQclear(res);
    res = NULL;
}

static void
processNotice(void *arg, const char *message)
{
    UNUSED(arg);
    UNUSED(message);

    // do nothing
}

int
main()
{
    int libpq_ver = PQlibVersion();
    printf("Version of libpq: %d\n", libpq_ver);

    conn = PQconnectdb(
        "user=postgres password=secretpass "
        "host=10.0.3.248 dbname=phonebook");

    if(PQstatus(conn) != CONNECTION_OK)
        terminate(1);

    // Don't output notices like:
    // NOTICE:  relation "phonebook" already exists, skipping
    // see http://stackoverflow.com/a/12504406/1565238
    PQsetNoticeProcessor(conn, processNotice, NULL);

    int server_ver = PQserverVersion(conn);
    char *user = PQuser(conn);
    char *db_name = PQdb(conn);

    printf("Server version: %d\n", server_ver);
    printf("User: %s\n", user);
    printf("Database name: %s\n", db_name);


    // same for insert, update, delete, begin, commit ...
    res = PQexec(conn, "CREATE TABLE IF NOT EXISTS phonebook "
            "(id SERIAL PRIMARY KEY, name VARCHAR(64), "
            "phone VARCHAR(64), last_changed TIMESTAMP)");
    if(PQresultStatus(res) != PGRES_COMMAND_OK)
        terminate(1);
    clearRes();

    res = PQexec(conn, "DELETE FROM phonebook");
    if(PQresultStatus(res) != PGRES_COMMAND_OK)
        terminate(1);
    clearRes();


    const char* query =
        "INSERT INTO phonebook (name, phone, last_changed) "
        " VALUES ($1, $2, now());";
    const char* params[2];

    for(int i = 0; ; i++)
    {
        const char* user = user_phone_arr[i][0];
        const char* phone = user_phone_arr[i][1];
        if(user == NULL || phone == NULL)
            break;

        params[0] = user;
        params[1] = phone;

        res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
        if(PQresultStatus(res) != PGRES_COMMAND_OK)
            terminate(1);
        clearRes();
    }


    res = PQexec(conn, "SELECT id, name, phone, last_changed FROM phonebook");
    if(PQresultStatus(res) != PGRES_TUPLES_OK)
        terminate(1);

    int ncols = PQnfields(res);
    printf("There are %d columns:", ncols);
    for(int i = 0; i < ncols; i++)
    {
        char *name = PQfname(res, i);
        printf(" %s", name);
    }
    printf("\n");


    int nrows = PQntuples(res);
    for(int i = 0; i < nrows; i++)
    {
        char* id = PQgetvalue(res, i, 0);
        char* name = PQgetvalue(res, i, 1);
        char* phone = PQgetvalue(res, i, 2);
        char* last_changed = PQgetvalue(res, i, 3);
        printf("Id: %s, Name: %s, Phone: %s, Last changed: %s\n",
            id, name, phone, last_changed);
    }

    printf("Total: %d rows\n", nrows);

    clearRes();
    terminate(0);
    return 0;
}

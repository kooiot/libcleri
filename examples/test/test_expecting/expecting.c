#include <time.h>
#include <string.h>
#include "expecting.h"

#define RETURN_REGEX_EXAMPLE 0
#define RETURN_REGEX_TYPE 1

/* prototyping of the user-defined function in the main.c */
const char * re_to_str(uint32_t gid, int choice);

/* Gives the element name */
const char * get_cleri_obj(cleri_t * object, int regex_choice)
{
    const char * expect;
    switch(object->tp)
        {
            case CLERI_TP_END_OF_STATEMENT:
                expect = "end_of_statement";
                break;
            case CLERI_TP_KEYWORD:
                expect = object->via.keyword->keyword;
                break;
            case CLERI_TP_TOKENS:
                expect = object->via.tokens->tokens;
                break;
            case CLERI_TP_TOKEN:
                expect = object->via.token->token;
                break;
            case CLERI_TP_REGEX:
                expect = re_to_str(object->gid, regex_choice);
                break;
            default:
                expect = "not found";
        }
    return expect;
}


const char * prt_expected(cleri_parse_t * pr)
{
    int rc;
    int count;
    /* create a second buffer for the expect string. If something
        goes wrong, redirection to only clean up the parse object.  */
    buffer_t * expect_buf = buffer_create();
    if (expect_buf == NULL)
        goto end_prt_expected;

    /* Form the expect string */
    for (count = 0; pr->expect != NULL; count++)
    {
        const char * expect = get_cleri_obj(pr->expect->cl_obj, RETURN_REGEX_TYPE);
        pr->expect = pr->expect->next;

        if (!count)
            rc = buffer_printf(expect_buf, "%s", expect);
        else if (pr->expect == NULL)
            rc += buffer_printf(expect_buf, " or %s", expect);
        else
            rc += buffer_printf(expect_buf, ", %s", expect);

        if(rc)
        {
            buffer_destroy(expect_buf);
            goto end_prt_expected;
        }
    }
    printf("Expected at position %lu: %s.\n", pr->pos, expect_buf->buf);
    buffer_destroy(expect_buf);

    if (count > 0)
    {
        /* choose a random object out of the expect olist and return it */
        int choice = (rand() * time(NULL)) % count; // seed
        cleri_parse_expect_start(pr);

        while (choice--)
        {
            pr->expect = pr->expect->next; // takes choice.
            if (pr->expect->cl_obj->tp == CLERI_TP_END_OF_STATEMENT)
                cleri_parse_expect_start(pr);
        }

        const char * expect = get_cleri_obj(pr->expect->cl_obj, RETURN_REGEX_EXAMPLE);

        return expect;
    }
end_prt_expected:
    return NULL;
}


/* Guided autocorrection */
void test_autocor(cleri_grammar_t * grammar, const char * str, buffer_t * buf)
{
    /* if parse function returns NULL, somthing went wrong and
        the program wil be aborted */
    cleri_parse_t * pr = cleri_parse(grammar, str);
    if (pr == NULL)
        abort();

    printf("Your string: '%s'\n", str);
    /* first time the buffer_printf() is used. Gets given string till 'pos'.
        If error in buffer_printf() then it would give a -1, in that case
        only clean up and leave the function. */
    int rc = buffer_printf(buf, "%.*s ",(int)pr->pos, str);
    if (rc)
        goto test_autocor;

    /* loops as long as the string turns out to be valid or if
        10 iterations haved passed */
    int trial;
    for (trial = 0; !pr->is_valid && trial < 10; trial++)
    {
        if (!trial)
        {
            printf("The string is not valid.\n");
            printf("I will try to create a valid string for you.\n\n");
        }
        else
            printf("But it's not quite right yet.\n");

        /* print the expected object in the olist and get an expected object
           which is randomly choosen and concatenate this to the buf string. */
        const char * expect = prt_expected(pr);
        rc += buffer_printf(buf, "%s ", expect);
        if (rc)
            goto test_autocor;
        printf("I will choose %s. This creates the following string: %s.\n\n", expect, buf->buf);
        cleri_parse_free(pr);
        pr = cleri_parse(grammar, buf->buf);
    }

    /* check if  */
    if (pr->is_valid)
        printf("Yeah the string is valid!\n");
    else
        printf("Sorry I've failed you...:(\n");

test_autocor:
    if (pr)
        cleri_parse_free(pr);
}





/* Guided autocorrection */
// char *  test_autocor2(cleri_grammar_t * grammar, const char * str, buffer_t * buf)
// {
//     cleri_parse_t * pr = cleri_parse(grammar, str);
//     if (pr == NULL)
//         abort();

//     int rc = buffer_printf(buf, "%.*s ",(int)pr->pos, str);
//     int trial;

//     for (trial = 0; !pr->is_valid && trial < 10; trial++)
//     {
//         int count;
//         for (count = 0; pr->expect != NULL; count++)
//         {
//             pr->expect = pr->expect->next;
//         }
//         if (!rc)
//         {
//             if (count > 0)
//             {
//                 int choice = (rand() * time(NULL)) % count; // seed
//                 cleri_parse_expect_start(pr);

//                 while (choice--)
//                 {
//                     pr->expect = pr->expect->next; // takes choice.
//                     if (pr->expect->cl_obj->tp == CLERI_TP_END_OF_STATEMENT)
//                         cleri_parse_expect_start(pr);
//                 }
//                 char * expect = get_cleri_obj(pr->expect->cl_obj, RETURN_REGEX_EXAMPLE);
//                 rc += buffer_printf(buf, "%s ", expect);
//             }
//         }
//         else
//         {
//             goto test_autocor;
//         }
//         cleri_parse_free(pr);
//         pr = cleri_parse(grammar, buf->buf);
//     }

// test_autocor:
//     if (pr)
//         cleri_parse_free(pr);
//     return buf->buf;
// }



/*

Functie die alles aan elkaar plakt.
functie die showt wat er allemaal geexpect wordt
functie die een valid string creert

*/
#include <stdio.h>
#include <ctype.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define INCL_BASE 1           /* remove for DOS compile */
#ifdef INCL_BASE
#include <os2.h>
#else
#include <dos.h>
#endif

#define TRUE 1
#define FALSE 0
#define page_size 16384

typedef struct text
          {
            int           length;
            unsigned char *value;
          } *text_ptr;

typedef struct value_header
           {
             char       type;
             union
               {
                 int      *boolean;
                 FILE     **dataset;
                 long     *integer;
                 double   *real;
                 text_ptr string;
               }        value_ptr;
           } *value_header_ptr;

typedef struct queue_node
                 {
                   value_header_ptr  argument_header_ptr;
                   struct queue_node *next;
                 } *queue_node_ptr;

typedef struct variable
                 {
                   char             *name;
                   queue_node_ptr   subscripts;
                   value_header_ptr variable_value_header_ptr;
                   struct variable  *predecessor_ptr;
                   struct variable  *smaller_successor_ptr;
                   struct variable  *larger_successor_ptr;
                 } *variable_ptr;

static value_header_ptr abs_header_ptr(queue_node_ptr,char *,int);
static void             add_argc(int);
static void             add_argv(int,char *);
static value_header_ptr add_terms(value_header_ptr,value_header_ptr);
static value_header_ptr and_factors(value_header_ptr,value_header_ptr);
static value_header_ptr atan_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr boolean_comparison(value_header_ptr,char *,
                         value_header_ptr);
static value_header_ptr char_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr concatenate_terms(value_header_ptr,value_header_ptr);
static value_header_ptr copy_of_arguments(value_header_ptr);
static queue_node_ptr   copy_of_queue(queue_node_ptr);
static value_header_ptr copy_of_subscripts(value_header_ptr);
static value_header_ptr cos_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr dataset_comparison(value_header_ptr,char *,
                         value_header_ptr);
static value_header_ptr date_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr divide_factors(value_header_ptr,value_header_ptr);
static value_header_ptr endfile_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr exec_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr exp_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr factor_header_ptr(int);
static value_header_ptr false_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr float_header_ptr(queue_node_ptr,char *,int);
static void             free_value(value_header_ptr);
static void             free_variables(void);
static value_header_ptr function_header_ptr(int);
static void             get_comparison_operator(char *);
static void             get_factor_operator(char *);
static value_header_ptr getchar_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr getint_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr getreal_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr getstring_header_ptr(queue_node_ptr,char *,int);
static void             get_source_char(void);
static void             get_term_operator(char *);
static void             get_token(void);
static value_header_ptr index_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr integer_comparison(value_header_ptr,char *,
                         value_header_ptr);
static void             interpret_assignment(int,queue_node_ptr);
static void             interpret_do(int);
static value_header_ptr interpret_expression(int);
static void             interpret_if(int);
static void             interpret_procedure(int,queue_node_ptr);
static void             interpret_statement(int);
static value_header_ptr length_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr lineno_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr log_header_ptr(queue_node_ptr,char *,int);
       int              main(int,char **);
static value_header_ptr mod_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr multiply_factors(value_header_ptr,value_header_ptr);
static value_header_ptr new_boolean_header_ptr(void);
static value_header_ptr new_dataset_header_ptr(void);
static value_header_ptr new_integer_header_ptr(void);
static value_header_ptr new_real_header_ptr(void);
static value_header_ptr new_string_header_ptr(unsigned);
static value_header_ptr open_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr or_terms(value_header_ptr,value_header_ptr);
static value_header_ptr ord_header_ptr(queue_node_ptr,char *,int);
static void             perform_close(int,queue_node_ptr);
static void             perform_clrscr(int,queue_node_ptr);
static void             perform_pliretc(int,queue_node_ptr);
static void             perform_print(int,queue_node_ptr);
static void             perform_putcrlf(int,queue_node_ptr);
static void             perform_troff(int,queue_node_ptr);
static void             perform_tron(int,queue_node_ptr);
static value_header_ptr pi_header_ptr(queue_node_ptr,char *,int);
static int              pli_strcmp(text_ptr,text_ptr);
static void             pli_strcpy(text_ptr,text_ptr);
static value_header_ptr pliretv_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr real_comparison(value_header_ptr,char *,
                         value_header_ptr);
static value_header_ptr repeat_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr simple_expression_header_ptr(int);
static value_header_ptr sin_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr sqr_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr sqrt_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr str_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr string_comparison(value_header_ptr,char *,
                         value_header_ptr);
static value_header_ptr string_header_ptr(int);
static value_header_ptr substr_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr subtract_terms(value_header_ptr,value_header_ptr);
static value_header_ptr sysin_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr sysprint_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr term_header_ptr(int);
static value_header_ptr time_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr translate_header_ptr(queue_node_ptr,char *,int);
static long             tree_balancer(long);
static value_header_ptr true_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr trunc_header_ptr(queue_node_ptr,char *,int);
static value_header_ptr unsigned_integer_header_ptr(void);
static value_header_ptr unsigned_number_header_ptr(int);
static value_header_ptr upper_header_ptr(queue_node_ptr,char *,int);
static int              variable_comparison(char *,queue_node_ptr,char *,
                         queue_node_ptr);
static value_header_ptr variable_header_ptr(char *,int,queue_node_ptr);
static value_header_ptr verify_header_ptr(queue_node_ptr,char *,int);

static int          fatal_error;
static char         identifier [256];
static char         page [page_size];
static int          page_index;
static long         page_num;
static int          return_code;
static char         source_char;
static long         source_column_num;
static int          source_eof;
FILE                *source_file;
static long         source_index;
static long         source_line_num;
static char         source_token [256];
static unsigned char substitute [256] =
         {
           0x8d,0x8f,0x47,0xba,0xcc,0x12,0x09,0x74,
           0xcb,0xf3,0xb4,0x88,0xf8,0xd1,0x08,0x4c,
           0xa1,0x32,0x48,0x98,0xbd,0xaa,0xea,0xa2,
           0x28,0xbc,0x66,0xe8,0xf4,0x5a,0x83,0x46,
           0xa4,0x0e,0x3b,0x3e,0x14,0x4d,0x1c,0x0a,
           0x92,0xfd,0x79,0xa8,0x67,0x41,0xe3,0x70,
           0xc2,0x56,0xdd,0x6c,0xbb,0x38,0x17,0xc1,
           0xae,0xb7,0x60,0x43,0x9e,0x34,0x22,0x7b,
           0xe6,0x61,0x54,0xa0,0x00,0xcf,0xd0,0x64,
           0xab,0x93,0xb6,0x86,0xee,0xdb,0x8e,0xb8,
           0x6f,0xb2,0x57,0xd5,0xe9,0x85,0x0d,0x5d,
           0x18,0xd9,0x82,0x6e,0x94,0x2b,0xb1,0xda,
           0x2d,0x0f,0x90,0xed,0xde,0x95,0x4b,0xf1,
           0x3d,0x3c,0x6b,0x2a,0xc9,0x21,0xfc,0xdf,
           0x16,0x3a,0x9d,0x7f,0x37,0xbf,0xc7,0x9a,
           0x25,0x49,0x0c,0xb9,0x91,0x03,0x97,0x35,
           0x39,0x2c,0x63,0x62,0x1e,0x73,0x7e,0xa3,
           0x45,0x71,0x44,0x40,0x9f,0xe2,0x13,0x3f,
           0x68,0xc6,0xc4,0xfa,0x4a,0x07,0x58,0x23,
           0xa5,0x4e,0x27,0x10,0x7c,0xd2,0x84,0x26,
           0x76,0xac,0x55,0xad,0x5e,0xe7,0x5b,0x04,
           0xd4,0xd7,0x89,0x96,0x0b,0x72,0xff,0xca,
           0xc0,0x6a,0x8a,0xfe,0x5c,0x99,0x01,0xd6,
           0x1f,0xdc,0xa7,0x78,0xf6,0x50,0x1b,0xe5,
           0xec,0x42,0x8b,0x36,0xcd,0x75,0x59,0x30,
           0x1d,0xe1,0x2e,0xbe,0x77,0xc5,0xb3,0xf2,
           0x11,0x52,0x53,0xe4,0x87,0x15,0x2f,0xf5,
           0x1a,0xb0,0x5f,0x9c,0xa6,0x69,0x05,0x7a,
           0xf7,0x6d,0xb5,0x24,0x81,0x80,0x9b,0xce,
           0x33,0xf9,0x65,0x19,0xeb,0xd3,0x31,0xef,
           0x20,0xf0,0x51,0x7d,0xa9,0x8c,0x02,0xaf,
           0x29,0xc3,0xc8,0xe0,0xfb,0xd8,0x4f,0x06
         };
static int          trace;
static variable_ptr variable_head;

int main(argc,argv)
 int  argc;
 char *argv[];
  {
    int arg_index;

    fatal_error=FALSE;
    return_code=0;
    trace=FALSE;
    if (argc >= 2)
      if ((source_file=fopen(argv[1],"rb")) == NULL)
        {
          fatal_error=TRUE;
          printf("Fatal error:  cannot open %s for input.\n",argv[1]);
        }
      else
        {
          source_char=' ';
          source_eof=FALSE;
          source_line_num=(long) 1;
          source_column_num=(long) 0;
          page_index=page_size;
          page_num=(long) -1;
          variable_head=NULL;
          arg_index=1;
          add_argc(argc-1);
          while ((! fatal_error) && (arg_index < argc))
            {
              add_argv(arg_index,argv[arg_index]);
              arg_index++;
            }
          while ((! source_eof) && (! fatal_error))
            {
              get_token();
              if (source_token[0] != ' ')
                interpret_statement(TRUE);
            }
          fclose(source_file);
          free_variables();
        }
    else
      {
        fatal_error=TRUE;
        printf("RUN interprets PL/I-like statements.\n\n");
        printf("  Usage:  RUN <source file>\n");
        printf("Example:  RUN C:\\USER\\WORK\\RUN.INP\n");
      }
    if (fatal_error)
      return_code=2000;
    return(return_code);
  }

static void add_argc(argc)
  int  argc;
    {
      if ((variable_head=(struct variable *)
       malloc((unsigned) sizeof(struct variable))) == NULL)
        {
          fatal_error=TRUE;
          printf("Fatal error:  out of memory at ");
          printf("line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      if (! fatal_error)
        {
          if (((*variable_head).name=malloc((unsigned) 5)) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *) variable_head);
              variable_head=NULL;
            }
        }
      if (! fatal_error)
        {
          strcpy((*variable_head).name,"ARGC");
          (*variable_head).subscripts=NULL;
          if (((*variable_head).variable_value_header_ptr
           =(struct value_header *)
           malloc((unsigned) sizeof(struct value_header)))
           == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((*variable_head).name);
              free((char *) variable_head);
              variable_head=NULL;
            }
        }
      if (! fatal_error)
        {
          (*((*variable_head).variable_value_header_ptr)).
           type='I';
          if (((*((*variable_head).variable_value_header_ptr)).
           value_ptr.integer=(long *)
           malloc((unsigned) sizeof(long))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *)
               (*variable_head).variable_value_header_ptr);
              free((*variable_head).name);
              free((char *) variable_head);
              variable_head=NULL;
            }
        }
      if (! fatal_error)
        {
          *((*((*variable_head).variable_value_header_ptr)).
           value_ptr.integer)=(long) argc;
          (*variable_head).predecessor_ptr=NULL;
          (*variable_head).smaller_successor_ptr=NULL;
          (*variable_head).larger_successor_ptr=NULL;
        }
      return;
    }

static void add_argv(arg_index,arg)
  int  arg_index;
  char *arg;
    {
      int              comparison;
      int              finished;
      variable_ptr     new_variable_ptr;
      variable_ptr     old_variable_ptr;

      if ((new_variable_ptr=(struct variable *)
       malloc((unsigned) sizeof(struct variable)))
       == NULL)
        {
          fatal_error=TRUE;
          printf("Fatal error:  out of memory at ");
          printf("line %ld, column %ld.\n",
          source_line_num,source_column_num);
        }
      if (! fatal_error)
        {
          if (((*new_variable_ptr).name=malloc((unsigned) 5)) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *) variable_head);
              variable_head=NULL;
            }
        }
      if (! fatal_error)
        {
          strcpy((*new_variable_ptr).name,"ARGV");
          if (((*new_variable_ptr).subscripts=(struct queue_node *)
           malloc((unsigned) sizeof(struct queue_node))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((*new_variable_ptr).name);
              free((char *) new_variable_ptr);
              new_variable_ptr=NULL;
            }
        }
      if (! fatal_error)
        {
          (*((*new_variable_ptr).subscripts)).next=NULL;
          if (((*((*new_variable_ptr).subscripts)).
           argument_header_ptr=(struct value_header *)
           malloc((unsigned) sizeof(struct value_header))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *) (*new_variable_ptr).subscripts);
              free((*new_variable_ptr).name);
              free((char *) new_variable_ptr);
              new_variable_ptr=NULL;
            }
        }
      if (! fatal_error)
        {
          (*((*((*new_variable_ptr).subscripts)).
           argument_header_ptr)).type='I';
          if (((*((*((*new_variable_ptr).subscripts)).
           argument_header_ptr)).value_ptr.integer=(long *)
           malloc((unsigned) sizeof(long))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *)
               (*((*new_variable_ptr).subscripts)).argument_header_ptr);
              free((char *) (*new_variable_ptr).subscripts);
              free((*new_variable_ptr).name);
              free((char *) new_variable_ptr);
              new_variable_ptr=NULL;
            }
        }
      if (! fatal_error)
        {
          *((*((*((*new_variable_ptr).subscripts)).
           argument_header_ptr)).value_ptr.integer)
           =tree_balancer((long) arg_index);
          if (((*new_variable_ptr).variable_value_header_ptr
           =(struct value_header *)
           malloc((unsigned) sizeof(struct value_header))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *)
               (*((*((*new_variable_ptr).subscripts)).
               argument_header_ptr)).value_ptr.integer);
              free((char *)
               (*((*new_variable_ptr).subscripts)).argument_header_ptr);
              free((char *) (*new_variable_ptr).subscripts);
              free((*new_variable_ptr).name);
              free((char *) new_variable_ptr);
              new_variable_ptr=NULL;
            }
        }
      if (! fatal_error)
        {
          (*((*new_variable_ptr).
           variable_value_header_ptr)).type='S';
          if (((*((*new_variable_ptr).variable_value_header_ptr)).
           value_ptr.string=(struct text *)
           malloc((unsigned) sizeof(struct text))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *)
               (*new_variable_ptr).
               variable_value_header_ptr);
              free((char *)
               (*((*((*new_variable_ptr).subscripts)).
               argument_header_ptr)).value_ptr.integer);
              free((char *)
               (*((*new_variable_ptr).subscripts)).argument_header_ptr);
              free((char *) (*new_variable_ptr).subscripts);
              free((*new_variable_ptr).name);
              free((char *) new_variable_ptr);
              new_variable_ptr=NULL;
            }
        }
      if (! fatal_error)
        {
          (*((*((*new_variable_ptr).
           variable_value_header_ptr)).
           value_ptr.string)).length=strlen(arg);
          if (((*((*((*new_variable_ptr).
           variable_value_header_ptr)).value_ptr.string)).value=
           malloc((unsigned) 1+strlen(arg))) == NULL)
            {
              fatal_error=TRUE;
              printf("Fatal error:  out of memory at ");
              printf("line %ld, column %ld.\n",
               source_line_num,source_column_num);
              free((char *) (*((*new_variable_ptr).
               variable_value_header_ptr)).value_ptr.string);
              free((char *)
               (*new_variable_ptr).variable_value_header_ptr);
              free((char *)
               (*((*((*new_variable_ptr).subscripts)).
               argument_header_ptr)).value_ptr.integer);
              free((char *)
               (*((*new_variable_ptr).subscripts)).argument_header_ptr);
              free((char *) (*new_variable_ptr).subscripts);
              free((*new_variable_ptr).name);
              free((char *) new_variable_ptr);
              new_variable_ptr=NULL;
            }
        }
      if (! fatal_error)
        {
          strcpy((*((*((*new_variable_ptr).
           variable_value_header_ptr)).value_ptr.string)).value,arg);
        }
      if (! fatal_error)
        {
          if (variable_head == NULL)
            {
              variable_head=new_variable_ptr;
              (*variable_head).predecessor_ptr=NULL;
              (*variable_head).smaller_successor_ptr=NULL;
              (*variable_head).larger_successor_ptr=NULL;
            }
          else
            {
              old_variable_ptr=variable_head;
              finished=FALSE;
              do
                {
                  comparison=variable_comparison((*new_variable_ptr).name,
                   (*new_variable_ptr).subscripts,(*old_variable_ptr).name,
                   (*old_variable_ptr).subscripts);
                  if (comparison < 0)
                    if ((*old_variable_ptr).smaller_successor_ptr == NULL)
                      {
                        (*new_variable_ptr).predecessor_ptr
                         =old_variable_ptr;
                        (*new_variable_ptr).
                         smaller_successor_ptr=NULL;
                        (*new_variable_ptr).
                         larger_successor_ptr=NULL;
                        (*old_variable_ptr).
                         smaller_successor_ptr
                         =new_variable_ptr;
                        finished=TRUE;
                      }
                    else
                      old_variable_ptr
                       =(*old_variable_ptr).smaller_successor_ptr;
                  else
                    if ((*old_variable_ptr).larger_successor_ptr
                     == NULL)
                      {
                        (*new_variable_ptr).predecessor_ptr
                         =old_variable_ptr;
                        (*new_variable_ptr).
                         smaller_successor_ptr=NULL;
                        (*new_variable_ptr).
                         larger_successor_ptr=NULL;
                        (*old_variable_ptr).
                         larger_successor_ptr
                         =new_variable_ptr;
                        finished=TRUE;
                      }
                    else
                      old_variable_ptr
                       =(*old_variable_ptr).larger_successor_ptr;
                }
              while (! finished);
            }
        }
      return;
    }

static void free_variables()
  {
    int            bypass_smaller_name;
    variable_ptr   current_ptr;
    int            finished;
    int            larger_predecessor_found;
    queue_node_ptr new_queue_head;
    variable_ptr   previous_ptr;
    queue_node_ptr queue_head;

    if (variable_head != NULL)
      {
        current_ptr=variable_head;
        finished=FALSE;
        bypass_smaller_name=FALSE;
        do
          {
            if (! bypass_smaller_name)
              while ((*current_ptr).smaller_successor_ptr != NULL)
                current_ptr=(*current_ptr).smaller_successor_ptr;
            free_value((*current_ptr).variable_value_header_ptr);
            if ((*current_ptr).larger_successor_ptr != NULL)
              {
                current_ptr=(*current_ptr).larger_successor_ptr;
                bypass_smaller_name=FALSE;
              }
            else
              {
                larger_predecessor_found=FALSE;
                do
                  {
                    if ((*current_ptr).predecessor_ptr == NULL)
                      finished=TRUE;
                    else
                      {
                        previous_ptr=current_ptr;
                        current_ptr=(*previous_ptr).predecessor_ptr;
                        if (variable_comparison((*current_ptr).name,
                         (*current_ptr).subscripts,(*previous_ptr).name,
                         (*previous_ptr).subscripts) > 0)
                          larger_predecessor_found=TRUE;
                        free((*previous_ptr).name);
                        queue_head=(*previous_ptr).subscripts;
                        while (queue_head != NULL)
                          {
                            new_queue_head=(*queue_head).next;
                            free_value((*queue_head).argument_header_ptr);
                            free((char *) queue_head);
                            queue_head=new_queue_head;
                          }
                        free((char *) previous_ptr);
                      }
                  }
               while ((! finished) && (! larger_predecessor_found));
               bypass_smaller_name=TRUE;
              }
          }
        while (! finished);
        free((*variable_head).name);
        queue_head=(*variable_head).subscripts;
        while (queue_head != NULL)
          {
            new_queue_head=(*queue_head).next;
            free_value((*queue_head).argument_header_ptr);
            free((char *) queue_head);
            queue_head=new_queue_head;
          }
        free((char *) variable_head);
      }
    return;
  }

static void get_source_char()
  {
    static int char_count;

    if (source_eof)
      source_char=' ';
    else
      {
        do
          {
            if (page_index >= page_size)
              {
                char_count=fread(&page[0],1,page_size,source_file);
                if (char_count < page_size)
                  page[char_count]=(char) 26;
                page_num++;
                page_index=0;
              }
            source_char=page[page_index];
            page_index++;
            if (source_char != (char) 26)
              {
                if (source_char == (char) 13)
                  {
                    source_line_num++;
                    source_column_num=(long) 0;
                  }
                else
                  {
                    if (source_char != (char) 10)
                      source_column_num++;
                  }
              }
          }
        while ((source_char != (char) 26)
        &&     ((source_char == (char) 13) || (source_char == (char) 10)));
        source_eof=(source_char == (char) 26);
        if (source_eof) source_char=' ';
      }
    return;
  }

static void get_token()
  {
    register int  token_index;

    while ((source_char == ' ')
    &&     (! source_eof))
      get_source_char();
    if (isalpha((int) source_char))
      {
        token_index=0;
        while ((isalnum((int) source_char) || (source_char == '_'))
        &&     (! source_eof))
          {
            if (token_index < 255)
              source_token[token_index++]=(char) toupper((int) source_char);
            get_source_char();
          }
        source_token[token_index]='\0';
      }
    else
      {
        source_token[0]=source_char;
        source_token[1]='\0';
        get_source_char();
      }
    return;
  }

static void free_value(header_ptr)
  value_header_ptr header_ptr;
    {
      if (header_ptr != NULL)
        {
          switch ((*header_ptr).type)
            {
              case 'B':
                free((char *) (*header_ptr).value_ptr.boolean);
                break;
              case 'D':
                free((char *) (*header_ptr).value_ptr.dataset);
                break;
              case 'I':
                free((char *) (*header_ptr).value_ptr.integer);
                break;
              case 'R':
                free((char *) (*header_ptr).value_ptr.real);
                break;
              default:
                free((*((*header_ptr).value_ptr.string)).value);
                free((char *) (*header_ptr).value_ptr.string);
                break;
            }
         free((char *) header_ptr);
        }
      return;
    }

static value_header_ptr new_boolean_header_ptr()
  {
    value_header_ptr result_header_ptr;

    if ((result_header_ptr=(struct value_header *)
     malloc((unsigned) sizeof(struct value_header))) == NULL)
      {
        fatal_error=TRUE;
        result_header_ptr=NULL;
        printf("Fatal error:  out of memory at ");
        printf("line %ld, column %ld.\n",
         source_line_num,source_column_num);
      }
    else
      {
        (*result_header_ptr).type='B';
        if (((*result_header_ptr).value_ptr.boolean=(int *)
         malloc((unsigned) sizeof(int))) == NULL)
          {
            fatal_error=TRUE;
            free((char *) result_header_ptr);
            result_header_ptr=NULL;
            printf("Fatal error:  out of memory at ");
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
        else
          *((*result_header_ptr).value_ptr.boolean)=TRUE;
      }
    return(result_header_ptr);
  }

static value_header_ptr new_dataset_header_ptr()
  {
    value_header_ptr result_header_ptr;

    if ((result_header_ptr=(struct value_header *)
     malloc((unsigned) sizeof(struct value_header))) == NULL)
      {
        fatal_error=TRUE;
        result_header_ptr=NULL;
        printf("Fatal error:  out of memory at ");
        printf("line %ld, column %ld.\n",
         source_line_num,source_column_num);
      }
    else
      {
        (*result_header_ptr).type='D';
        if (((*result_header_ptr).value_ptr.dataset=(FILE **)
         malloc((unsigned) sizeof(FILE *))) == NULL)
          {
            fatal_error=TRUE;
            free((char *) result_header_ptr);
            result_header_ptr=NULL;
            printf("Fatal error:  out of memory at ");
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
        else
          *((*result_header_ptr).value_ptr.dataset)=NULL;
      }
    return(result_header_ptr);
  }

static value_header_ptr new_integer_header_ptr()
  {
    value_header_ptr result_header_ptr;

    if ((result_header_ptr=(struct value_header *)
     malloc((unsigned) sizeof(struct value_header))) == NULL)
      {
        fatal_error=TRUE;
        result_header_ptr=NULL;
        printf("Fatal error:  out of memory at ");
        printf("line %ld, column %ld.\n",
         source_line_num,source_column_num);
      }
    else
      {
        (*result_header_ptr).type='I';
        if (((*result_header_ptr).value_ptr.integer=(long *)
         malloc((unsigned) sizeof(long))) == NULL)
          {
            fatal_error=TRUE;
            free((char *) result_header_ptr);
            result_header_ptr=NULL;
            printf("Fatal error:  out of memory at ");
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
        else
          *((*result_header_ptr).value_ptr.integer)=0;
      }
    return(result_header_ptr);
  }

static value_header_ptr new_real_header_ptr()
  {
    value_header_ptr result_header_ptr;

    if ((result_header_ptr=(struct value_header *)
     malloc((unsigned) sizeof(struct value_header))) == NULL)
      {
        fatal_error=TRUE;
        result_header_ptr=NULL;
        printf("Fatal error:  out of memory at ");
        printf("line %ld, column %ld.\n",
         source_line_num,source_column_num);
      }
    else
      {
        (*result_header_ptr).type='R';
        if (((*result_header_ptr).value_ptr.real=(double *)
         malloc((unsigned) sizeof(double))) == NULL)
          {
            fatal_error=TRUE;
            free((char *) result_header_ptr);
            result_header_ptr=NULL;
            printf("Fatal error:  out of memory at ");
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
        else
          *((*result_header_ptr).value_ptr.real)=0.0;
      }
    return(result_header_ptr);
  }

static value_header_ptr new_string_header_ptr(string_length)
  unsigned string_length;
    {
      value_header_ptr result_header_ptr;

      if (string_length > (unsigned) 32767)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
        "Fatal error:  string length exceeds 32767 at line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      else
        if ((result_header_ptr=(struct value_header *)
         malloc((unsigned) sizeof(struct value_header))) == NULL)
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf("Fatal error:  out of memory at ");
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
        else
          {
            (*result_header_ptr).type='S';
            if (((*result_header_ptr).value_ptr.string=(struct text *)
             malloc((unsigned) sizeof(struct text))) == NULL)
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf("Fatal error:  out of memory at ");
                printf("line %ld, column %ld.\n",
                 source_line_num,source_column_num);
              }
            else
              {
                (*((*result_header_ptr).value_ptr.string)).length
                 =string_length;
                if (((*((*result_header_ptr).value_ptr.string)).value=
                 (unsigned char *) malloc((unsigned) (1+string_length)))
                 == NULL)
                  {
                    fatal_error=TRUE;
                    free((char *) result_header_ptr);
                    result_header_ptr=NULL;
                    printf("Fatal error:  out of memory at ");
                    printf("line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
                else
                  *((*((*result_header_ptr).value_ptr.string)).value)
                   =(unsigned char) '\0';
              }
          }
      return(result_header_ptr);
    }

static int pli_strcmp(string_1,string_2)
  text_ptr string_1;
  text_ptr string_2;
    {
      unsigned char char_1;
      unsigned char char_2;
      register int  char_index;
      unsigned char *char_ptr_1;
      unsigned char *char_ptr_2;
      int           length_1;
      int           length_2;
      int           result;

      result=0;
      char_index=0;
      char_ptr_1=(*string_1).value;
      char_ptr_2=(*string_2).value;
      length_1=(*string_1).length;
      length_2=(*string_2).length;
      while ((result == 0)
      &&     (char_index < length_1)
      &&     (char_index < length_2))
         {
           char_1=*char_ptr_1;
           char_2=*char_ptr_2;
           if (char_1 < char_2)
             result=-1;
           else
             if (char_1 > char_2)
               result=1;
             else
               {
                 char_index++;
                 char_ptr_1++;
                 char_ptr_2++;
               }
        }
      char_2=(unsigned char) ' ';
      while ((result == 0)
      &&     (char_index < length_1))
        {
          char_1=*char_ptr_1;
          if (char_1 < char_2)
            result=-1;
          else
            if (char_1 > char_2)
              result=1;
            else
              {
                char_index++;
                char_ptr_1++;
              }
        }
      char_1=(unsigned char) ' ';
      while ((result == 0)
      &&     (char_index < length_2))
        {
          char_2=*char_ptr_2;
          if (char_1 < char_2)
            result=-1;
          else
            if (char_1 > char_2)
              result=1;
            else
              {
                char_index++;
                char_ptr_2++;
              }
        }
      return(result);
    }

static void pli_strcpy(string_1,string_2)
  text_ptr string_1;
  text_ptr string_2;
    {
      register int  char_index;
      unsigned char *char_ptr_1;
      unsigned char *char_ptr_2;
      int           string_length;

      char_ptr_1=(*string_1).value;
      char_ptr_2=(*string_2).value;
      string_length=(*string_2).length;
      for (char_index=0; char_index < string_length; char_index++)
        {
          *char_ptr_1=*char_ptr_2;
          char_ptr_1++;
          char_ptr_2++;
        }
      *char_ptr_1=(unsigned char) '\0';
      return;
    }

static value_header_ptr string_header_ptr(evaluate)
  int evaluate;
    {
      value_header_ptr new_result_header_ptr;
      value_header_ptr result_header_ptr;
      int              string_index;
      unsigned         string_length;
      int              string_terminated;

      string_index=-1;
      string_length=(unsigned) 0;
      result_header_ptr=new_string_header_ptr((unsigned) 0);
      string_terminated=FALSE;
      while ((! fatal_error) && (! string_terminated) && (! source_eof))
        {
          get_source_char();
          if (source_char == '\'')
            {
              get_source_char();
              if (source_char == '\'')
                {
                  string_length++;
                  new_result_header_ptr=new_string_header_ptr(string_length);
                  if (! fatal_error)
                    {
                      pli_strcpy((*new_result_header_ptr).value_ptr.string,
                       (*result_header_ptr).value_ptr.string);
                      string_index++;
                      (*((*new_result_header_ptr).value_ptr.string)).value[
                       string_index]=(unsigned char) source_char;
                      (*((*new_result_header_ptr).value_ptr.string)).value[
                       string_length]=(unsigned char) '\0';
                      free_value(result_header_ptr);
                      result_header_ptr=new_result_header_ptr;
                    }
                }
              else
                string_terminated=TRUE;
            }
          else
            {
              string_length++;
              new_result_header_ptr=new_string_header_ptr(string_length);
              if (! fatal_error)
                {
                  pli_strcpy((*new_result_header_ptr).value_ptr.string,
                   (*result_header_ptr).value_ptr.string);
                  string_index++;
                  (*((*new_result_header_ptr).value_ptr.string)).value[
                   string_index]=(unsigned char) source_char;
                  (*((*new_result_header_ptr).value_ptr.string)).value[
                   string_length]=(unsigned char) '\0';
                  free_value(result_header_ptr);
                  result_header_ptr=new_result_header_ptr;
                }
            }
        }
      if (! evaluate)
        {
          free_value(result_header_ptr);
          result_header_ptr=NULL;
        }
      return(result_header_ptr);
    }

static long tree_balancer(argument)
  long argument;
    {
      union
        {
          struct
            {
              unsigned char number_1;
              unsigned char number_2;
              unsigned char number_3;
              unsigned char number_4;
            } byte;
          struct
            {
              unsigned int number_01 : 1;
              unsigned int number_02 : 1;
              unsigned int number_03 : 1;
              unsigned int number_04 : 1;
              unsigned int number_05 : 1;
              unsigned int number_06 : 1;
              unsigned int number_07 : 1;
              unsigned int number_08 : 1;
              unsigned int number_09 : 1;
              unsigned int number_10 : 1;
              unsigned int number_11 : 1;
              unsigned int number_12 : 1;
              unsigned int number_13 : 1;
              unsigned int number_14 : 1;
              unsigned int number_15 : 1;
              unsigned int number_16 : 1;
              unsigned int number_17 : 1;
              unsigned int number_18 : 1;
              unsigned int number_19 : 1;
              unsigned int number_20 : 1;
              unsigned int number_21 : 1;
              unsigned int number_22 : 1;
              unsigned int number_23 : 1;
              unsigned int number_24 : 1;
              unsigned int number_25 : 1;
              unsigned int number_26 : 1;
              unsigned int number_27 : 1;
              unsigned int number_28 : 1;
              unsigned int number_29 : 1;
              unsigned int number_30 : 1;
              unsigned int number_31 : 1;
              unsigned int number_32 : 1;
            } bit;
        } intermediate;
      union
        {
          long       signed_long;
          struct
            {
              unsigned char number_1;
              unsigned char number_2;
              unsigned char number_3;
              unsigned char number_4;
            } byte;
          struct
            {
              unsigned int number_01 : 1;
              unsigned int number_02 : 1;
              unsigned int number_03 : 1;
              unsigned int number_04 : 1;
              unsigned int number_05 : 1;
              unsigned int number_06 : 1;
              unsigned int number_07 : 1;
              unsigned int number_08 : 1;
              unsigned int number_09 : 1;
              unsigned int number_10 : 1;
              unsigned int number_11 : 1;
              unsigned int number_12 : 1;
              unsigned int number_13 : 1;
              unsigned int number_14 : 1;
              unsigned int number_15 : 1;
              unsigned int number_16 : 1;
              unsigned int number_17 : 1;
              unsigned int number_18 : 1;
              unsigned int number_19 : 1;
              unsigned int number_20 : 1;
              unsigned int number_21 : 1;
              unsigned int number_22 : 1;
              unsigned int number_23 : 1;
              unsigned int number_24 : 1;
              unsigned int number_25 : 1;
              unsigned int number_26 : 1;
              unsigned int number_27 : 1;
              unsigned int number_28 : 1;
              unsigned int number_29 : 1;
              unsigned int number_30 : 1;
              unsigned int number_31 : 1;
              unsigned int number_32 : 1;
            } bit;
        } result;
      register int round;

      result.signed_long=argument;
      for (round=1; round <= 8; round++)
        {
          intermediate.bit.number_01=result.bit.number_04;
          intermediate.bit.number_02=result.bit.number_29;
          intermediate.bit.number_03=result.bit.number_06;
          intermediate.bit.number_04=result.bit.number_09;
          intermediate.bit.number_05=result.bit.number_26;
          intermediate.bit.number_06=result.bit.number_25;
          intermediate.bit.number_07=result.bit.number_16;
          intermediate.bit.number_08=result.bit.number_15;
          intermediate.bit.number_09=result.bit.number_24;
          intermediate.bit.number_10=result.bit.number_31;
          intermediate.bit.number_11=result.bit.number_02;
          intermediate.bit.number_12=result.bit.number_18;
          intermediate.bit.number_13=result.bit.number_32;
          intermediate.bit.number_14=result.bit.number_03;
          intermediate.bit.number_15=result.bit.number_20;
          intermediate.bit.number_16=result.bit.number_30;
          intermediate.bit.number_17=result.bit.number_08;
          intermediate.bit.number_18=result.bit.number_27;
          intermediate.bit.number_19=result.bit.number_13;
          intermediate.bit.number_20=result.bit.number_11;
          intermediate.bit.number_21=result.bit.number_01;
          intermediate.bit.number_22=result.bit.number_17;
          intermediate.bit.number_23=result.bit.number_10;
          intermediate.bit.number_24=result.bit.number_05;
          intermediate.bit.number_25=result.bit.number_07;
          intermediate.bit.number_26=result.bit.number_14;
          intermediate.bit.number_27=result.bit.number_19;
          intermediate.bit.number_28=result.bit.number_23;
          intermediate.bit.number_29=result.bit.number_21;
          intermediate.bit.number_30=result.bit.number_28;
          intermediate.bit.number_31=result.bit.number_12;
          intermediate.bit.number_32=result.bit.number_22;
          result.byte.number_1=substitute[intermediate.byte.number_1];
          result.byte.number_2=substitute[intermediate.byte.number_2];
          result.byte.number_3=substitute[intermediate.byte.number_3];
          result.byte.number_4=substitute[intermediate.byte.number_4];
        }
      return(result.signed_long);
    }

static int variable_comparison(name_1,queue_head_1,name_2,queue_head_2)
  char           *name_1;
  queue_node_ptr queue_head_1;
  char           *name_2;
  queue_node_ptr queue_head_2;
    {
      int       boolean_1;
      int       boolean_2;
      union  {
               FILE *file_ptr;
               long address;
             }  dataset_1;
      union  {
               FILE *file_ptr;
               long address;
             }  dataset_2;
      long      integer_1;
      long      integer_2;
      double    real_1;
      double    real_2;
      int       result;
      char      type_1;
      char      type_2;

      result=strcmp(name_1,name_2);
      if (result == 0)
        {
          while ((result == 0)
          &&     (queue_head_1 != NULL)
          &&     (queue_head_2 != NULL))
            {
              type_1=(*((*queue_head_1).argument_header_ptr)).type;
              type_2=(*((*queue_head_2).argument_header_ptr)).type;
              if (type_1 < type_2)
                result=-1;
              else
                if (type_1 > type_2)
                  result=1;
                else
                  switch (type_1)
                    {
                      case 'B':
                        boolean_1=*((*((*queue_head_1).argument_header_ptr)).
                         value_ptr.boolean);
                        boolean_2=*((*((*queue_head_2).argument_header_ptr)).
                         value_ptr.boolean);
                        if (boolean_1)
                          {
                            if (! boolean_2)
                              result=1;
                          }
                        else
                          {
                            if (boolean_2)
                              result=-1;
                          }
                        break;
                      case 'D':
                        dataset_1.file_ptr
                         =*((*((*queue_head_1).argument_header_ptr)).
                         value_ptr.dataset);
                        dataset_2.file_ptr
                         =*((*((*queue_head_2).argument_header_ptr)).
                         value_ptr.dataset);
                        if (dataset_1.address < dataset_2.address)
                          result=-1;
                        else
                          {
                            if (dataset_1.address > dataset_2.address)
                              result=1;
                          }
                        break;
                      case 'I':
                        integer_1=*((*((*queue_head_1).argument_header_ptr)).
                         value_ptr.integer);
                        integer_2=*((*((*queue_head_2).argument_header_ptr)).
                         value_ptr.integer);
                        if (integer_1 < integer_2)
                          result=-1;
                        else
                          {
                            if (integer_1 > integer_2)
                              result=1;
                          }
                        break;
                      case 'R':
                        real_1=*((*((*queue_head_1).argument_header_ptr)).
                         value_ptr.real);
                        real_2=*((*((*queue_head_2).argument_header_ptr)).
                         value_ptr.real);
                        if (real_1 < real_2)
                          result=-1;
                        else
                          {
                            if (real_1 > real_2)
                              result=1;
                          }
                        break;
                      default:
                        result=pli_strcmp(
                         (*((*queue_head_1).argument_header_ptr)).
                         value_ptr.string,
                         (*((*queue_head_2).argument_header_ptr)).
                         value_ptr.string);
                        break;
                    }
              queue_head_1=(*queue_head_1).next;
              queue_head_2=(*queue_head_2).next;
            }
          if (result == 0)
            {
              if (queue_head_1 == NULL)
                {
                  if (queue_head_2 != NULL)
                    result=-1;
                }
              else
                {
                  if (queue_head_2 == NULL)
                    result=1;
                }
            }
        }
      return(result);
    }

static value_header_ptr copy_of_arguments(argument_header_ptr)
 value_header_ptr argument_header_ptr;
  {
    value_header_ptr result_header_ptr;

    if (argument_header_ptr == NULL)
      result_header_ptr=NULL;
    else
      switch ((*argument_header_ptr).type)
        {
          case 'B':
            result_header_ptr=new_boolean_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.boolean)
              =*((*argument_header_ptr).value_ptr.boolean);
            break;
          case 'D':
            result_header_ptr=new_dataset_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.dataset)
              =*((*argument_header_ptr).value_ptr.dataset);
            break;
          case 'I':
            result_header_ptr=new_integer_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.integer)
              =*((*argument_header_ptr).value_ptr.integer);
            break;
          case 'R':
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.real)
              =*((*argument_header_ptr).value_ptr.real);
            break;
          default:
            result_header_ptr
             =new_string_header_ptr((unsigned)
             (*((*argument_header_ptr).value_ptr.string)).length);
            if (! fatal_error)
             pli_strcpy((*(result_header_ptr)).value_ptr.string,
              (*argument_header_ptr).value_ptr.string);
            break;
        }
    return(result_header_ptr);
  }

static value_header_ptr copy_of_subscripts(argument_header_ptr)
 value_header_ptr argument_header_ptr;
  {
    value_header_ptr result_header_ptr;

    if (argument_header_ptr == NULL)
      result_header_ptr=NULL;
    else
      switch ((*argument_header_ptr).type)
        {
          case 'B':
            result_header_ptr=new_boolean_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.boolean)
              =*((*argument_header_ptr).value_ptr.boolean);
            break;
          case 'D':
            result_header_ptr=new_dataset_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.dataset)
              =*((*argument_header_ptr).value_ptr.dataset);
            break;
          case 'I':
            result_header_ptr=new_integer_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.integer)
              =tree_balancer(*((*argument_header_ptr).value_ptr.integer));
            break;
          case 'R':
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
             *((*(result_header_ptr)).value_ptr.real)
              =*((*argument_header_ptr).value_ptr.real);
            break;
          default:
            result_header_ptr
             =new_string_header_ptr((unsigned)
             (*((*argument_header_ptr).value_ptr.string)).length);
            if (! fatal_error)
             pli_strcpy((*(result_header_ptr)).value_ptr.string,
              (*argument_header_ptr).value_ptr.string);
            break;
        }
    return(result_header_ptr);
  }

static queue_node_ptr copy_of_queue(queue_head)
  queue_node_ptr queue_head;
    {
      queue_node_ptr copy_queue_head;
      queue_node_ptr copy_queue_tail;
      queue_node_ptr new_copy_queue_tail;
      queue_node_ptr new_queue_head;

      copy_queue_head=NULL;
      copy_queue_tail=NULL;
      while ((queue_head != NULL) && (! fatal_error))
        {
          new_queue_head=(*queue_head).next;
          if (copy_queue_head == NULL)
            if ((copy_queue_head=(queue_node_ptr)
             malloc((unsigned) sizeof(struct queue_node))) == NULL)
              {
                fatal_error=TRUE;
                printf(
                 "Fatal error:  out of memory at line %ld, column %ld.\n",
                 source_line_num,source_column_num);
              }
            else
              {
                copy_queue_tail=copy_queue_head;
                (*copy_queue_head).next=NULL;
                (*copy_queue_head).argument_header_ptr
                 =copy_of_subscripts((*queue_head).argument_header_ptr);
              }
          else
            if ((new_copy_queue_tail=(queue_node_ptr)
             malloc((unsigned) sizeof(struct queue_node))) == NULL)
              {
                fatal_error=TRUE;
                printf(
                 "Fatal error:  out of memory at line %ld, column %ld.\n",
                 source_line_num,source_column_num);
              }
            else
              {
                (*new_copy_queue_tail).next=NULL;
                (*copy_queue_tail).next=new_copy_queue_tail;
                copy_queue_tail=new_copy_queue_tail;
                (*new_copy_queue_tail).argument_header_ptr
                 =copy_of_subscripts((*queue_head).argument_header_ptr);
              }
          queue_head=new_queue_head;
        }
      return(copy_queue_head);
    }

static value_header_ptr variable_header_ptr(variable_name,evaluate,queue_head)
  char           *variable_name;
  int            evaluate;
  queue_node_ptr queue_head;
    {
      int              comparison;
      int              finished;
      queue_node_ptr   new_queue_copy;
      variable_ptr     parameter_ptr;
      queue_node_ptr   queue_copy;
      value_header_ptr result_header_ptr;
      int              variable_found;

      if (evaluate)
        {
          variable_found=FALSE;
          if (variable_head != NULL)
            {
              parameter_ptr=variable_head;
              queue_copy=copy_of_queue(queue_head);
              finished=FALSE;
              do
                {
                  comparison=variable_comparison(variable_name,queue_copy,
                   (*parameter_ptr).name,(*parameter_ptr).subscripts);
                  if (comparison < 0)
                    if ((*parameter_ptr).smaller_successor_ptr == NULL)
                      finished=TRUE;
                    else
                      parameter_ptr=(*parameter_ptr).smaller_successor_ptr;
                  else
                    if (comparison > 0)
                      if ((*parameter_ptr).larger_successor_ptr == NULL)
                        finished=TRUE;
                      else
                        parameter_ptr=(*parameter_ptr).larger_successor_ptr;
                    else
                      {
                        variable_found=TRUE;
                        result_header_ptr
                         =copy_of_arguments(
                         (*parameter_ptr).variable_value_header_ptr);
                        finished=TRUE;
                      }
                }
              while (! finished);
              while (queue_copy != NULL)
                {
                  new_queue_copy=(*queue_copy).next;
                  free_value((*queue_copy).argument_header_ptr);
                  free((char *) queue_copy);
                  queue_copy=new_queue_copy;
                }
            }
          if (! variable_found)
            result_header_ptr=NULL;
        }
      else
        result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr unsigned_integer_header_ptr()
    {
      unsigned long    result;
      value_header_ptr result_header_ptr;
      unsigned long    tem_unsigned_long;

      result=(unsigned long) 0;
      do
        {
          if ((source_char >= '0') && (source_char <= '9'))
            {
              tem_unsigned_long=(unsigned long) source_char;
              tem_unsigned_long-=(unsigned long) '0';
              result*=(unsigned long) 10;
              result+=tem_unsigned_long;
              if (result <= 0x7fffffff)
                get_source_char();
            }
        }
      while ((source_char >= '0') && (source_char <= '9')
      &&     (result <= 0x7fffffff));
      if (result <= 0x7fffffff)
        {
          result_header_ptr=new_integer_header_ptr();
          if (! fatal_error)
            *((*result_header_ptr).value_ptr.integer)=(long) result;
        }
      else
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  integer constant too big at line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      return(result_header_ptr);
    }

static value_header_ptr unsigned_number_header_ptr(evaluate)
  int evaluate;
    {
      value_header_ptr exponent_header_ptr;
      char             exponent_sign;
      long             exponent_value;
      double           factor;
      value_header_ptr result_header_ptr;
      double           tem_real_1;
      double           tem_real_2;

      result_header_ptr=unsigned_integer_header_ptr();
      if (! fatal_error)
        {
          if (source_char == '.')
            {
              tem_real_1=(double) *((*result_header_ptr).value_ptr.integer);
              free_value(result_header_ptr);
              result_header_ptr=new_real_header_ptr();
              if (! fatal_error)
                {
                  *((*result_header_ptr).value_ptr.real)=tem_real_1;
                  get_source_char();
                  if (isdigit((int) source_char))
                    {
                      factor=1.0;
                      while (isdigit((int) source_char))
                        {
                          factor=factor/10.0;
                          tem_real_2=(float) source_char;
                          tem_real_2-=(float) '0';
                          tem_real_1+=factor*tem_real_2;
                          get_source_char();
                        }
                      *((*result_header_ptr).value_ptr.real)=tem_real_1;
                    }
                  else
                    {
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
                  "Fatal error:  decimal part of real number is missing at\n");
                      printf(
                  "line %ld, column %ld.\n",source_line_num,source_column_num);
                    }
                }
            }
          if (! fatal_error)
            {
              if ((source_char == 'e') || (source_char == 'E'))
                {
                  if ((*result_header_ptr).type == 'I')
                    {
                      tem_real_1
                       =(double) *((*result_header_ptr).value_ptr.integer);
                      free_value(result_header_ptr);
                      result_header_ptr=new_real_header_ptr();
                      if (! fatal_error)
                        *((*result_header_ptr).value_ptr.real)=tem_real_1;
                    }
                  if (! fatal_error)
                    {
                      get_source_char();
                      if (source_eof)
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
                           "Fatal error:  file ends before real number ");
                          printf(
                           "completed.\n");
                        }
                    }
                  if (! fatal_error)
                    {
                      if ((source_char == '+')
                      ||  (source_char == '-'))
                        {
                          exponent_sign=source_char;
                          get_source_char();
                        }
                      else
                        exponent_sign=' ';
                    }
                  if (! fatal_error)
                    {
                      if (source_eof)
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
                           "Fatal error:  file ends before real number ");
                          printf(
                           "completed.\n");
                        }
                    }
                  if (! fatal_error)
                    {
                      if (! isdigit((int) source_char))
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
                "Fatal error:  nonnumeric exponent at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                    }
                  if (! fatal_error)
                    exponent_header_ptr=unsigned_integer_header_ptr();
                  if (! fatal_error)
                    {
                      if (*((*exponent_header_ptr).value_ptr.integer)
                       > (long) 37)
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          free_value(exponent_header_ptr);
                          result_header_ptr=NULL;
                          printf(
                "Fatal error:  exponent too large at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                    }
                  if (! fatal_error)
                    {
                      tem_real_1=1.0;
                      exponent_value
                       =*((*exponent_header_ptr).value_ptr.integer);
                      free_value(exponent_header_ptr);
                      while (exponent_value > (long) 0)
                        {
                          exponent_value--;
                          tem_real_1*=10.0;
                        }
                      if (exponent_sign == '-')
                        tem_real_1=1.0/tem_real_1;
                      if (*((*result_header_ptr).value_ptr.real) != 0.0)
                        {
                          tem_real_2=(log(tem_real_1)
                           +log(fabs(*((*result_header_ptr).value_ptr.real))))
                           /log(10.0);
                          if (tem_real_2 < -37.0)
                            *((*result_header_ptr).value_ptr.real)=0.0;
                          else
                            if (tem_real_2 > 37.0)
                              {
                                fatal_error=TRUE;
                                free_value(result_header_ptr);
                                result_header_ptr=NULL;
                                printf(
                     "Fatal error:  real too large at line %ld, column %ld.\n",
                                 source_line_num,source_column_num);
                              }
                            else
                              *((*result_header_ptr).value_ptr.real)
                               *=tem_real_1;
                        }
                    }
                }
            }
        }
      if (! evaluate)
        {
          free_value(result_header_ptr);
          result_header_ptr=NULL;
        }
      return(result_header_ptr);
    }

static value_header_ptr abs_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                result_header_ptr=new_integer_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.integer)
                   =labs(*((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer));
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    *((*result_header_ptr).value_ptr.real)
                     =fabs(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real));
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr atan_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      double           tem_real;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                tem_real=(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.real)=atan(tem_real);
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    *((*result_header_ptr).value_ptr.real)
                     =atan(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real));
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr char_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      long             tem_integer;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                tem_integer=*((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                if (((long) 0 <= tem_integer) && (tem_integer <= (long) 255))
                  {
                    result_header_ptr=new_string_header_ptr((unsigned) 1);
                    if (! fatal_error)
                      {
                        *((*((*result_header_ptr).value_ptr.string)).value)
                         =(unsigned char) tem_integer;
                        *((*((*result_header_ptr).value_ptr.string)).value+1)
                         =(unsigned char) '\0';
                      }
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
                  "Fatal error:  argument to CHAR is not between 0 and 255\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                 "Fatal error:  argument to CHAR is other than an integer\n");
                printf("     on line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr cos_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      double           tem_real;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                tem_real=(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.real)=cos(tem_real);
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    *((*result_header_ptr).value_ptr.real)
                     =cos(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real));
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr date_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      unsigned char    *char_ptr_1;
      char             *char_ptr_2;
      char             date_and_time [26];
      long             elapsed_time;
      struct tm        *local_time;
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_string_header_ptr(6);
            if (! fatal_error)
              {
                char_ptr_1=(*((*result_header_ptr).value_ptr.string)).value;
                time(&elapsed_time);
                local_time=localtime(&elapsed_time);
                strcpy(&date_and_time[0],asctime(local_time));
                strncpy((char *) char_ptr_1,&date_and_time[22],2);
                *(char_ptr_1+2)=(unsigned char) '\0';
                date_and_time[7]='\0';
                char_ptr_2=&date_and_time[4];
                if      (strcmp(char_ptr_2,"Jan") == 0)
                  strcat((char *) char_ptr_1,"01");
                else if (strcmp(char_ptr_2,"Feb") == 0)
                  strcat((char *) char_ptr_1,"02");
                else if (strcmp(char_ptr_2,"Mar") == 0)
                  strcat((char *) char_ptr_1,"03");
                else if (strcmp(char_ptr_2,"Apr") == 0)
                  strcat((char *) char_ptr_1,"04");
                else if (strcmp(char_ptr_2,"May") == 0)
                  strcat((char *) char_ptr_1,"05");
                else if (strcmp(char_ptr_2,"Jun") == 0)
                  strcat((char *) char_ptr_1,"06");
                else if (strcmp(char_ptr_2,"Jul") == 0)
                  strcat((char *) char_ptr_1,"07");
                else if (strcmp(char_ptr_2,"Aug") == 0)
                  strcat((char *) char_ptr_1,"08");
                else if (strcmp(char_ptr_2,"Sep") == 0)
                  strcat((char *) char_ptr_1,"09");
                else if (strcmp(char_ptr_2,"Oct") == 0)
                  strcat((char *) char_ptr_1,"10");
                else if (strcmp(char_ptr_2,"Nov") == 0)
                  strcat((char *) char_ptr_1,"11");
                else
                  strcat((char *) char_ptr_1,"12");
                strncat((char *) char_ptr_1,&date_and_time[8],2);
              }
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr endfile_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      FILE             *file;
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          result_header_ptr=new_boolean_header_ptr();
          if (! fatal_error)
            {
              if (feof(stdin) == 0)
                *((*result_header_ptr).value_ptr.boolean)=FALSE;
              else
                *((*result_header_ptr).value_ptr.boolean)=TRUE;
            }
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'D')
              {
                file=*((*((*queue_head).argument_header_ptr)).
                 value_ptr.dataset);
                result_header_ptr=new_boolean_header_ptr();
                if (! fatal_error)
                  {
                    if (feof(file) == 0)
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
           "Fatal error:  argument to ENDFILE is other than a file pointer\n");
                printf("     on line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr exec_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'S')
              {
                result_header_ptr=new_boolean_header_ptr();
                if (! fatal_error)
                  {
                    if (system(
                     (char *) (*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).value) == 0)
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
"Fatal error:  other than a string supplied as argument to function \"%s\"\n",
                 function_name);
                printf("     on line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr exp_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      double           tem_real_1;
      double           tem_real_2;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                tem_real_1=(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  {
                    tem_real_2=tem_real_1/log(10.0);
                    if (tem_real_2 < -37.0)
                      *((*result_header_ptr).value_ptr.real)=0.0;
                    else
                      if (tem_real_2 > 37.0)
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
       "Fatal error:  argument to EXP is too large at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)=exp(tem_real_1);
                  }
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  tem_real_1=*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real);
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    {
                      tem_real_2=tem_real_1/log(10.0);
                      if (tem_real_2 < -37.0)
                        *((*result_header_ptr).value_ptr.real)=0.0;
                      else
                        if (tem_real_2 > 37.0)
                          {
                            fatal_error=TRUE;
                            free_value(result_header_ptr);
                            result_header_ptr=NULL;
                            printf(
       "Fatal error:  argument to EXP is too large at line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                        else
                          *((*result_header_ptr).value_ptr.real)
                           =exp(tem_real_1);
                    }
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr false_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_boolean_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.boolean)=FALSE;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr float_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      int              status;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.real)
                   =(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    *((*result_header_ptr).value_ptr.real)
                     =*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real);
                }
              else
                if ((*((*queue_head).argument_header_ptr)).type == 'B')
                  {
                    result_header_ptr=new_real_header_ptr();
                    if (! fatal_error)
                      {
                        if (*((*((*queue_head).argument_header_ptr)).
                         value_ptr.boolean))
                          *((*result_header_ptr).value_ptr.real)=1.0;
                        else
                          *((*result_header_ptr).value_ptr.real)=0.0;
                      }
                  }
                else
                  if ((*((*queue_head).argument_header_ptr)).type == 'S')
                    {
                      result_header_ptr=new_real_header_ptr();
                      if (! fatal_error)
                        {
                          status=sscanf((char *)
                           (*((*((*queue_head).argument_header_ptr)).
                           value_ptr.string)).value,"%lf",
                           (*result_header_ptr).value_ptr.real);
                          if ((status == EOF) || (status == 0))
                            {
                              fatal_error=TRUE;
                              free_value(result_header_ptr);
                              result_header_ptr=NULL;
                              printf(
                   "Fatal error:  argument to FLOAT cannot be converted on\n");
                              printf("     line %ld, column %ld.\n",
                               source_line_num,source_column_num);
                            }
                        }
                    }
                  else
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
 "Fatal error:  argument to FLOAT is other than Boolean, number, or string\n");
                      printf("     on line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr getchar_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      int              current_char;
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'D')
              {
                current_char=fgetc(
                 *((*((*queue_head).argument_header_ptr)).value_ptr.dataset));
                if (current_char == EOF)
                  {
                    result_header_ptr=new_string_header_ptr(0);
                    if (! fatal_error)
                      *((*((*result_header_ptr).value_ptr.string)).value)
                       =(unsigned char) '\0';
                  }
                else
                  {
                    result_header_ptr=new_string_header_ptr(1);
                    if (! fatal_error)
                      {
                        *((*((*result_header_ptr).value_ptr.string)).value)
                         =(unsigned char) current_char;
                        *((*((*result_header_ptr).value_ptr.string)).value+1)
                         =(unsigned char) '\0';
                      }
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
        "Fatal error:  argument to GETCHAR is other than a file pointer on\n");
                printf("     line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      else
        if (evaluate)
          {
            current_char=fgetc(stdin);
            if (current_char == EOF)
              {
                result_header_ptr=new_string_header_ptr(0);
                if (! fatal_error)
                  *((*((*result_header_ptr).value_ptr.string)).value)
                   =(unsigned char) '\0';
              }
            else
              {
                result_header_ptr=new_string_header_ptr(1);
                if (! fatal_error)
                  {
                    *((*((*result_header_ptr).value_ptr.string)).value)
                     =(unsigned char) current_char;
                    *((*((*result_header_ptr).value_ptr.string)).value+1)
                     =(unsigned char) '\0';
                  }
              }
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr getint_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      int              num_fields_read;
      value_header_ptr result_header_ptr;
      long             tem_integer;

      if (queue_head != NULL)
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'D')
              {
                num_fields_read=fscanf(
                 *((*((*queue_head).argument_header_ptr)).value_ptr.dataset),
                 "%I",&tem_integer);
                if (num_fields_read == 0)
                  tem_integer=(long) 0;
                result_header_ptr=new_integer_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.integer)=tem_integer;
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
         "Fatal error:  argument to GETINT is other than a file pointer on\n");
                printf("     line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      else
        if (evaluate)
          {
            num_fields_read=scanf("%I",&tem_integer);
            if (num_fields_read == 0)
              tem_integer=(long) 0;
            result_header_ptr=new_integer_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.integer)=tem_integer;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr getreal_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      int              num_fields_read;
      value_header_ptr result_header_ptr;
      double           tem_real;

      if (queue_head != NULL)
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'D')
              {
                num_fields_read=fscanf(
                 *((*((*queue_head).argument_header_ptr)).value_ptr.dataset),
                 "%lf",&tem_real);
                if (num_fields_read == 0)
                  tem_real=0.0;
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.real)=tem_real;
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
        "Fatal error:  argument to GETREAL is other than a file pointer on\n");
                printf("     line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      else
        if (evaluate)
          {
            num_fields_read=scanf("%lf",&tem_real);
            if (num_fields_read == 0)
              tem_real=0.0;
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.real)=tem_real;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr getstring_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      int              current_char;
      value_header_ptr new_result_header_ptr;
      value_header_ptr result_header_ptr;
      unsigned         string_length;

      if (queue_head != NULL)
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'D')
              {
                string_length=0;
                result_header_ptr=new_string_header_ptr(string_length);
                if (! fatal_error)
                  {
                    *((*((*result_header_ptr).value_ptr.string)).value)
                     =(unsigned char) '\0';
                    do
                      {
                        current_char=fgetc(
                         *((*((*queue_head).argument_header_ptr)).value_ptr.
                         dataset));
                        if ((current_char != EOF)
                        &&  (current_char != 10))
                          {
                            string_length++;
                            new_result_header_ptr
                             =new_string_header_ptr(string_length);
                            if (! fatal_error)
                              {
                                pli_strcpy(
                                 (*new_result_header_ptr).value_ptr.string,
                                 (*result_header_ptr).value_ptr.string);
                                (*((*new_result_header_ptr).value_ptr.string)).
                                 value[string_length-1]
                                 =(unsigned char) current_char;
                                (*((*new_result_header_ptr).value_ptr.string)).
                                 value[string_length]=(unsigned char) '\0';
                                free_value(result_header_ptr);
                                result_header_ptr=new_result_header_ptr;
                              }
                          }
                      }
                    while ((! fatal_error)
                    &&     (current_char != EOF)
                    &&     (current_char != 10));
                    if ((*((*result_header_ptr).value_ptr.string)).value[
                     string_length-1] == (unsigned) 13)
                      {
                        string_length--;
                        (*((*result_header_ptr).value_ptr.string)).value[
                         string_length]=(unsigned char) '\0';
                        (*((*result_header_ptr).value_ptr.string)).length
                         =string_length;
                      }
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
      "Fatal error:  argument to GETSTRING is other than a file pointer on\n");
                printf("     line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      else
        if (evaluate)
          {
            string_length=0;
            result_header_ptr=new_string_header_ptr(string_length);
            if (! fatal_error)
              {
                *((*((*result_header_ptr).value_ptr.string)).value)
                 =(unsigned char) '\0';
                fflush(stdin);
                do
                  {
                    current_char=fgetc(stdin);
                    if ((current_char != EOF)
                    &&  (current_char != 10))
                      {
                        string_length++;
                        new_result_header_ptr
                         =new_string_header_ptr(string_length);
                        if (! fatal_error)
                          {
                            pli_strcpy(
                             (*new_result_header_ptr).value_ptr.string,
                             (*result_header_ptr).value_ptr.string);
                            (*((*new_result_header_ptr).value_ptr.string)).
                             value[string_length-1]
                             =(unsigned char) current_char;
                            (*((*new_result_header_ptr).value_ptr.string)).
                             value[string_length]=(unsigned char) '\0';
                            free_value(result_header_ptr);
                            result_header_ptr=new_result_header_ptr;
                          }
                      }
                  }
                while ((! fatal_error)
                &&     (current_char != EOF)
                &&     (current_char != 10));
                if ((*((*result_header_ptr).value_ptr.string)).value[
                 string_length-1] == (unsigned) 13)
                  {
                    string_length--;
                    (*((*result_header_ptr).value_ptr.string)).value[
                     string_length]=(unsigned char) '\0';
                    (*((*result_header_ptr).value_ptr.string)).length
                     =string_length;
                  }
              }
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr index_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      long             char_index_2;
      unsigned char    *char_ptr_1;
      unsigned char    *char_ptr_2;
      unsigned char    *char_ptr_3;
      unsigned char    *char_ptr_4;
      unsigned char    *char_ptr_5;
      long             length_1;
      long             length_2;
      int              match_found;
      long             num_trials;
      long             result;
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'S')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'S')
                  {
                    result_header_ptr=new_integer_header_ptr();
                    if (! fatal_error)
                      {
                        char_ptr_1=(*((*((*queue_head).
                         argument_header_ptr)).value_ptr.string)).value;
                        length_1=(*((*((*queue_head).
                         argument_header_ptr)).value_ptr.string)).length;
                        char_ptr_2=(*((*((*((*queue_head).next)).
                         argument_header_ptr)).value_ptr.string)).value;
                        length_2=(*((*((*((*queue_head).next)).
                         argument_header_ptr)).value_ptr.string)).length;
                        if (length_2 == (long) 0)
                          *((*result_header_ptr).value_ptr.integer)=(long) 0;
                        else
                          {
                            num_trials=length_1-length_2+(long) 1;
                            char_ptr_3=char_ptr_1;
                            match_found=FALSE;
                            result=(long) 1;
                            while ((result <= num_trials)
                            &&     (! match_found))
                              {
                                char_ptr_4=char_ptr_2;
                                char_index_2=(long) 1;
                                char_ptr_5=char_ptr_3;
                                while ((char_index_2 <= length_2)
                                &&     (*char_ptr_4 == *char_ptr_5))
                                  {
                                    char_ptr_4++;
                                    char_ptr_5++;
                                    char_index_2++;
                                  }
                                if (char_index_2 > length_2)
                                  match_found=TRUE;
                                else
                                  {
                                    char_ptr_3++;
                                    result++;
                                  }
                              }
                            if (match_found)
                              *((*result_header_ptr).value_ptr.integer)=result;
                            else
                              *((*result_header_ptr).value_ptr.integer)
                               =(long) 0;
                          }
                      }
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
            "Fatal error:  second argument to INDEX is other than a string\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
             "Fatal error:  first argument to INDEX is other than a string\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
               function_name);
              printf("     line %ld, column %ld.\n",source_line_num,
               source_column_num);
            }
      return(result_header_ptr);
    }

static value_header_ptr length_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'S')
              {
                result_header_ptr=new_integer_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.integer)=(long)
                   (*((*((*queue_head).argument_header_ptr)).
                   value_ptr.string)).length;
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                 "Fatal error:  argument to LENGTH is other than a string\n");
                printf("     on line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr lineno_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_integer_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.integer)=source_line_num;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr log_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      double           tem_real;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                tem_real=(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  {
                    if (tem_real > 0.0)
                      *((*result_header_ptr).value_ptr.real)=log(tem_real);
                    else
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
    "Fatal error:  argument to LOG is not positive at line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                  }
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  tem_real=*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real);
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    {
                      if (tem_real > 0.0)
                        *((*result_header_ptr).value_ptr.real)=log(tem_real);
                      else
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
    "Fatal error:  argument to LOG is not positive at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                    }
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr mod_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      long             tem_int_1;
      long             tem_int_2;
      long             tem_int_3;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'I')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'I')
                  if (*((*((*((*queue_head).next)).argument_header_ptr)).
                   value_ptr.integer) == 0)
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
                       "Fatal error:  second argument to MOD is zero\n");
                      printf("     on line %ld, column %ld.\n",source_line_num,
                       source_column_num);
                    }
                  else
                    {
                      result_header_ptr=new_integer_header_ptr();
                      if (! fatal_error)
                        {
                          tem_int_1
                           =*((*((*queue_head).argument_header_ptr)).
                           value_ptr.integer);
                          tem_int_2
                           =*((*((*((*queue_head).next)).argument_header_ptr)).
                           value_ptr.integer);
                          tem_int_3=tem_int_1/tem_int_2;
                          tem_int_3*=tem_int_2;
                          *((*result_header_ptr).value_ptr.integer)
                           =tem_int_1-tem_int_3;
                        }
                    }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
            "Fatal error:  second argument to MOD is other than an integer\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
             "Fatal error:  first argument to MOD is other than an integer\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
               function_name);
              printf("     line %ld, column %ld.\n",source_line_num,
               source_column_num);
            }
      return(result_header_ptr);
    }

static value_header_ptr open_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'S')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'S')
                  {
                    result_header_ptr=new_dataset_header_ptr();
                    if (! fatal_error)
                      {
                        *((*result_header_ptr).value_ptr.dataset)
                         =fopen((*((*((*queue_head).
                         argument_header_ptr)).value_ptr.string)).value,
                         (*((*((*((*queue_head).next)).
                         argument_header_ptr)).value_ptr.string)).value);
                        if (*((*result_header_ptr).value_ptr.dataset) == NULL)
                          {
                            fatal_error=TRUE;
                            free_value(result_header_ptr);
                            result_header_ptr=NULL;
                            printf(
                        "Fatal error:  cannot OPEN \"%s\" in mode \"%s\" on\n",
                             (*((*((*queue_head).argument_header_ptr)).
                             value_ptr.string)).value,
                             (*((*((*((*queue_head).next)).
                             argument_header_ptr)).value_ptr.string)).value);
                            printf("     on line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                      }
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
            "Fatal error:  second argument to OPEN is other than a string\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
             "Fatal error:  first argument to OPEN is other than a string\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
               function_name);
              printf("     line %ld, column %ld.\n",source_line_num,
               source_column_num);
            }
      return(result_header_ptr);
    }

static value_header_ptr ord_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'S')
              if ((*((*((*queue_head).argument_header_ptr)).value_ptr.string)).
               length == 0)
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
                   "Fatal error:  argument to ORD has length zero\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
              else
                {
                  result_header_ptr=new_integer_header_ptr();
                  if (! fatal_error)
                    *((*result_header_ptr).value_ptr.integer)
                     =(long) *((*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).value);
                }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                 "Fatal error:  argument to ORD is other than a string\n");
                printf("     on line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr pi_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.real)=4.0*atan(1.0);
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr pliretv_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_integer_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.integer)=return_code;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr repeat_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      long             char_index;
      unsigned char    *char_ptr_1;
      unsigned char    *char_ptr_2;
      unsigned char    *char_ptr_3;
      value_header_ptr result_header_ptr;
      long             tem_int_1;
      long             tem_int_2;
      long             result_length;
      double           tem_real_1;
      double           tem_real_2;
      double           tem_real_3;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'S')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'I')
                  {
                    char_ptr_1=(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).value;
                    tem_int_1=(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).length;
                    tem_int_2
                     =*((*((*((*queue_head).next)).
                     argument_header_ptr)).value_ptr.integer);
                    if (tem_int_2 >= (long) 0)
                      if (tem_int_1 == 0)
                        {
                          result_header_ptr
                           =new_string_header_ptr((unsigned) 0);
                          if (! fatal_error)
                            *((*((*result_header_ptr).value_ptr.string)).value)
                            =(unsigned char) '\0';
                        }
                      else
                        {
                          tem_real_1=(double) tem_int_1;
                          tem_real_2=(double) tem_int_2;
                          tem_real_3
                           =(log(fabs(tem_real_1))+log(fabs(tem_real_2+1.0)))
                           /log(2.0);
                          if (tem_real_3 >= 15.0)
                            {
                              fatal_error=TRUE;
                              result_header_ptr=NULL;
                             printf(
                          "Fatal error:  result of REPEAT too long on line\n");
                              printf(
                               "     %ld, column %ld.\n",source_line_num,
                               source_column_num);
                            }
                          else
                            {
                              result_length=tem_int_1;
                              result_length*=(tem_int_2+(long) 1);
                              result_header_ptr
                               =new_string_header_ptr(
                               (unsigned) result_length);
                              if (! fatal_error)
                                {
                                  char_ptr_3
                                   =(*((*result_header_ptr).value_ptr.string)).
                                   value;
                                  while (tem_int_2 >= (long) 0)
                                    {
                                      char_ptr_2=char_ptr_1;
                                      for (char_index=(long) 0;
                                       char_index < tem_int_1;
                                       char_index++)
                                        {
                                          *char_ptr_3=*char_ptr_2;
                                          char_ptr_3++;
                                          char_ptr_2++;
                                        }
                                      tem_int_2--;
                                    }
                                }
                            }
                        }
                    else
                      result_header_ptr=copy_of_arguments(
                       (*queue_head).argument_header_ptr);
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
         "Fatal error:  second argument to REPEAT is other than an integer\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
            "Fatal error:  first argument to REPEAT is other than a string\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
               function_name);
              printf("     line %ld, column %ld.\n",source_line_num,
               source_column_num);
            }
      return(result_header_ptr);
    }

static value_header_ptr sin_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      double           tem_real;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                tem_real=(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.real)=sin(tem_real);
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    *((*result_header_ptr).value_ptr.real)
                     =sin(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.real));
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr sqr_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      long             tem_integer;
      double           tem_real_1;
      double           tem_real_2;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                result_header_ptr=new_integer_header_ptr();
                if (! fatal_error)
                  {
                    tem_integer
                     =*((*((*queue_head).argument_header_ptr)).
                     value_ptr.integer);
                    if (tem_integer == 0)
                      *((*result_header_ptr).value_ptr.integer)=0;
                    else
                      {
                        tem_real_1=(double) tem_integer;
                        tem_real_2=2.0*log(fabs(tem_real_1))/log(2.0);
                        if (tem_real_2 >= 31.0)
                          {
                            fatal_error=TRUE;
                            free_value(result_header_ptr);
                            result_header_ptr=NULL;
                            printf(
          "Fatal error:  argument to SQR too large on line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                        else
                          *((*result_header_ptr).value_ptr.integer)
                           =tem_integer*tem_integer;
                      }
                  }
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    {
                      tem_real_1
                       =*((*((*queue_head).argument_header_ptr)).
                       value_ptr.real);
                      if (tem_real_1 == 0.0)
                        *((*result_header_ptr).value_ptr.real)=0.0;
                      else
                        {
                          tem_real_2=2.0*log(fabs(tem_real_1))/log(10.0);
                          if (tem_real_2 < -37.0)
                            *((*result_header_ptr).value_ptr.real)=0.0;
                          else
                            if (tem_real_2 > 37.0)
                              {
                                fatal_error=TRUE;
                                free_value(result_header_ptr);
                                result_header_ptr=NULL;
                                printf(
          "Fatal error:  argument to SQR too large on line %ld, column %ld.\n",
                                 source_line_num,source_column_num);
                              }
                            else
                              *((*result_header_ptr).value_ptr.real)
                               =tem_real_1*tem_real_1;
                        }
                    }
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr sqrt_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      double           tem_real;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
                {
                  tem_real=(double) *((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
                  if (tem_real < 0.0)
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
                     "Fatal error:  argument to function \"%s\" is negative\n",
                       function_name);
                      printf("     on line %ld, column %ld.\n",source_line_num,
                       source_column_num);
                    }
                  else
                    {
                      tem_real=sqrt(tem_real);
                      result_header_ptr=new_real_header_ptr();
                      if (! fatal_error)
                        *((*result_header_ptr).value_ptr.real)=tem_real;
                    }
                }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  tem_real=*((*((*queue_head).argument_header_ptr)).
                   value_ptr.real);
                  if (tem_real < 0.0)
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
                     "Fatal error:  argument to function \"%s\" is negative\n",
                       function_name);
                      printf("     on line %ld, column %ld.\n",source_line_num,
                       source_column_num);
                    }
                  else
                    {
                      tem_real=sqrt(tem_real);
                      result_header_ptr=new_real_header_ptr();
                      if (! fatal_error)
                        *((*result_header_ptr).value_ptr.real)=tem_real;
                    }
                }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  other than a number supplied as argument to function \"%s\"\n",
                   function_name);
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr str_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      char             buffer [256];
      value_header_ptr result_header_ptr;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            switch ((*((*queue_head).argument_header_ptr)).type)
              {
                case 'B':
                  if
                  (*((*((*queue_head).argument_header_ptr)).value_ptr.boolean))
                    {
                      result_header_ptr=new_string_header_ptr((unsigned) 4);
                      if (! fatal_error)
                        strcpy((char *)
                         (*((*result_header_ptr).value_ptr.string)).value,
                         "TRUE");
                    }
                  else
                    {
                      result_header_ptr=new_string_header_ptr((unsigned) 5);
                      if (! fatal_error)
                        strcpy((char *)
                         (*((*result_header_ptr).value_ptr.string)).value,
                         "FALSE");
                    }
                  break;
                case 'D':
                  buffer[sprintf(buffer,"%p",
                   *((*((*queue_head).argument_header_ptr)).value_ptr.
                   dataset))]='\0';
                  result_header_ptr
                   =new_string_header_ptr((unsigned) strlen(buffer));
                  if (! fatal_error)
                    strcpy((char *)
                     (*((*result_header_ptr).value_ptr.string)).value,buffer);
                  break;
                case 'I':
                  buffer[sprintf(buffer,"%ld",
                   *((*((*queue_head).argument_header_ptr)).value_ptr.integer))]
                   ='\0';
                  result_header_ptr
                   =new_string_header_ptr((unsigned) strlen(buffer));
                  if (! fatal_error)
                    strcpy((char *)
                     (*((*result_header_ptr).value_ptr.string)).value,buffer);
                  break;
                case 'R':
                  buffer[sprintf(buffer,"%lG",
                   *((*((*queue_head).argument_header_ptr)).value_ptr.real))]
                   ='\0';
                  result_header_ptr
                   =new_string_header_ptr((unsigned) strlen(buffer));
                  if (! fatal_error)
                    strcpy((char *)
                     (*((*result_header_ptr).value_ptr.string)).value,buffer);
                  break;
                default:
                  result_header_ptr=new_string_header_ptr((unsigned)
                   (*((*((*queue_head).argument_header_ptr)).value_ptr.
                   string)).length);
                  if (! fatal_error)
                    pli_strcpy((*result_header_ptr).value_ptr.string,
                     (*((*queue_head).argument_header_ptr)).value_ptr.string);
                  break;
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr substr_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      unsigned char    *char_ptr;
      unsigned char    *destination_ptr;
      long             final_column;
      long             num_columns;
      int              offset;
      value_header_ptr result_header_ptr;
      unsigned char    *source_ptr;
      long             starting_column;
      int              string_length;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
             result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'S')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'I')
                  {
                    char_ptr=(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).value;
                    string_length=(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).length;
                    starting_column
                     =*((*((*((*queue_head).next)).
                     argument_header_ptr)).value_ptr.integer);
                    if (starting_column <= (long) 0)
                      {
                        fatal_error=TRUE;
                        result_header_ptr=NULL;
                        printf(
                  "Fatal error:  second argument to SUBSTR is not positive\n");
                        printf(
                         "     on line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                    else
                      if (starting_column > string_length)
                        {
                          fatal_error=TRUE;
                          result_header_ptr=NULL;
                          printf(
 "Fatal error:  second argument to SUBSTR exceeds length of first argument\n");
                          printf(
                           "     on line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        {
                          num_columns=string_length-starting_column+(long) 1;
                          result_header_ptr
                           =new_string_header_ptr((unsigned) num_columns);
                          if (! fatal_error)
                            {
                              offset=(int) starting_column;
                              offset--;
                              source_ptr=char_ptr+offset;
                              destination_ptr
                               =(*((*result_header_ptr).value_ptr.string)).
                               value;
                              while (num_columns > 0)
                                {
                                   *destination_ptr=*source_ptr;
                                   source_ptr++;
                                   destination_ptr++;
                                   num_columns--;
                                }
                              *destination_ptr=(unsigned char) '\0';
                            }
                        }
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
         "Fatal error:  second argument to SUBSTR is other than an integer\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
            "Fatal error:  first argument to SUBSTR is other than a string\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            if ((*((*((*queue_head).next)).next)).next == NULL)
              if (evaluate)
                if ((*((*queue_head).argument_header_ptr)).type == 'S')
                  if ((*((*((*queue_head).next)).argument_header_ptr)).type
                   == 'I')
                    if ((*((*((*((*queue_head).next)).next)).
                     argument_header_ptr)).type == 'I')
                      {
                        char_ptr=(*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).value;
                        string_length
                         =(*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).length;
                        starting_column
                         =*((*((*((*queue_head).next)).
                         argument_header_ptr)).value_ptr.integer);
                        if (starting_column <= (long) 0)
                          {
                            fatal_error=TRUE;
                            result_header_ptr=NULL;
                            printf(
                  "Fatal error:  second argument to SUBSTR is not positive\n");
                            printf(
                             "     on line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                        else
                          {
                            num_columns
                             =*((*((*((*((*queue_head).next)).next)).
                             argument_header_ptr)).value_ptr.integer);
                            if (num_columns == (long) 0)
                              {
                                result_header_ptr=new_string_header_ptr(0);
                                if (! fatal_error)
                                  *((*((*result_header_ptr).value_ptr.string)).
                                   value)=(unsigned char) '\0';
                              }
                            else
                              {
                                final_column=starting_column+num_columns-1;
                                if (final_column > string_length)
                                  {
                                    fatal_error=TRUE;
                                    result_header_ptr=NULL;
                                    printf(
             "Fatal error:  SUBSTRing extends beyond end of first argument\n");
                                    printf(
                                     "     on line %ld, column %ld.\n",
                                     source_line_num,source_column_num);
                                  }
                                else
                                  if (final_column < starting_column)
                                    {
                                      fatal_error=TRUE;
                                      result_header_ptr=NULL;
                                      printf(
                   "Fatal error:  third argument to SUBSTR is not positive\n");
                                      printf(
                                       "     on line %ld, column %ld.\n",
                                       source_line_num,source_column_num);
                                    }
                                  else
                                    {
                                      result_header_ptr
                                       =new_string_header_ptr(
                                       (unsigned) num_columns);
                                      if (! fatal_error)
                                        {
                                          offset=(int) starting_column;
                                          offset--;
                                          source_ptr=char_ptr+offset;
                                          destination_ptr
                                           =(*((*result_header_ptr).
                                           value_ptr.string)).value;
                                          while (final_column
                                           >= starting_column)
                                            {
                                              *destination_ptr=*source_ptr;
                                              source_ptr++;
                                              destination_ptr++;
                                              starting_column++;
                                            }
                                          *destination_ptr='\0';
                                        }
                                    }
                              }
                          }
                      }
                    else
                      {
                        fatal_error=TRUE;
                        result_header_ptr=NULL;
                        printf(
          "Fatal error:  third argument to SUBSTR is other than an integer\n");
                        printf("     on line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                  else
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
         "Fatal error:  second argument to SUBSTR is other than an integer\n");
                      printf("     on line %ld, column %ld.\n",source_line_num,
                       source_column_num);
                    }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
            "Fatal error:  first argument to SUBSTR is other than a string\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                result_header_ptr=NULL;
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
                 function_name);
                printf("     line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
      return(result_header_ptr);
    }

static value_header_ptr sysin_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_dataset_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.dataset)=stdin;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr sysprint_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_dataset_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.dataset)=stdout;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr time_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      unsigned char    *char_ptr;
      char             date_and_time [26];
      long             elapsed_time;
      struct tm        *local_time;
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_string_header_ptr((unsigned) 9);
            if (! fatal_error)
              {
                char_ptr=(*((*result_header_ptr).value_ptr.string)).value;
                time(&elapsed_time);
                local_time=localtime(&elapsed_time);
                strcpy(&date_and_time[0],asctime(local_time));
                *char_ptr=(unsigned char) date_and_time[11];
                char_ptr++;
                *char_ptr=(unsigned char) date_and_time[12];
                char_ptr++;
                *char_ptr=(unsigned char) date_and_time[14];
                char_ptr++;
                *char_ptr=(unsigned char) date_and_time[15];
                char_ptr++;
                *char_ptr=(unsigned char) date_and_time[17];
                char_ptr++;
                *char_ptr=(unsigned char) date_and_time[18];
                char_ptr++;
                *char_ptr=(unsigned char) '0';
                char_ptr++;
                *char_ptr=(unsigned char) '0';
                char_ptr++;
                *char_ptr=(unsigned char) '0';
                char_ptr++;
                *char_ptr=(unsigned char) '\0';
              }
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr translate_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      unsigned         char_index_1;
      int              char_index_2;
      unsigned         char_index_3;
      unsigned char    *char_ptr_1;
      unsigned char    *char_ptr_2;
      unsigned char    *char_ptr_3;
      unsigned char    *char_ptr_4;
      unsigned char    *char_ptr_5;
      value_header_ptr result_header_ptr;
      unsigned         length_1;
      int              length_2;
      unsigned         length_3;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
             result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'S')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'S')
                  {
                    char_ptr_1
                     =(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).value;
                    length_1
                     =(*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).length;
                    char_ptr_2=(*((*((*((*queue_head).next)).
                     argument_header_ptr)).value_ptr.string)).value;
                    length_2=(*((*((*((*queue_head).next)).
                     argument_header_ptr)).value_ptr.string)).length;
                    result_header_ptr=new_string_header_ptr((unsigned)
                     (*((*((*queue_head).argument_header_ptr)).
                     value_ptr.string)).length);
                    if (! fatal_error)
                      {
                        char_ptr_5
                         =(*((*result_header_ptr).value_ptr.string)).value;
                        char_index_1=(unsigned) 1;
                        while (char_index_1 <= length_1)
                          {
                            char_index_2=(int) *char_ptr_1;
                            if (char_index_2 < length_2)
                              *char_ptr_5=*(char_ptr_2+char_index_2);
                            else
                              *char_ptr_5=(unsigned char) ' ';
                            char_index_1++;
                            char_ptr_1++;
                            char_ptr_5++;
                          }
                      }
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
        "Fatal error:  second argument to TRANSLATE is other than a string\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
         "Fatal error:  first argument to TRANSLATE is other than a string\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            if ((*((*((*queue_head).next)).next)).next == NULL)
              if (evaluate)
                if ((*((*queue_head).argument_header_ptr)).type == 'S')
                  if ((*((*((*queue_head).next)).argument_header_ptr)).type
                   == 'S')
                    if ((*((*((*((*queue_head).next)).next)).
                     argument_header_ptr)).type == 'S')
                      {
                        char_ptr_1
                         =(*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).value;
                        length_1
                         =(unsigned) (*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).length;
                        char_ptr_2=(*((*((*((*queue_head).next)).
                         argument_header_ptr)).value_ptr.string)).value;
                        length_2=(*((*((*((*queue_head).next)).
                         argument_header_ptr)).value_ptr.string)).length;
                        char_ptr_3=(*((*((*((*((*queue_head).next)).next)).
                         argument_header_ptr)).value_ptr.string)).value;
                        length_3
                         =(unsigned) (*((*((*((*((*queue_head).next)).next)).
                         argument_header_ptr)).value_ptr.string)).length;
                        result_header_ptr=new_string_header_ptr((unsigned)
                         (*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).length);
                        if (! fatal_error)
                          {
                            char_ptr_5
                             =(*((*result_header_ptr).value_ptr.string)).value;
                            char_index_1=(unsigned) 1;
                            while (char_index_1 <= length_1)
                              {
                                char_index_2=0;
                                char_ptr_4=char_ptr_3;
                                char_index_3=1;
                                while ((char_index_3 <= length_3)
                                &&     (*char_ptr_4 != *char_ptr_1))
                                  {
                                    char_ptr_4++;
                                    char_index_2++;
                                    char_index_3++;
                                  }
                                if (char_index_3 <= length_3)
                                  {
                                    if ((long) char_index_2 >= length_2)
                                      *char_ptr_5=(unsigned char) ' ';
                                    else
                                      *char_ptr_5=*(char_ptr_2+char_index_2);
                                  }
                                else
                                  *char_ptr_5=*char_ptr_1;
                                char_ptr_1++;
                                char_ptr_5++;
                                char_index_1++;
                              }
                            *char_ptr_5=(unsigned char) '\0';
                          }
                      }
                    else
                      {
                        fatal_error=TRUE;
                        result_header_ptr=NULL;
                        printf(
         "Fatal error:  third argument to TRANSLATE is other than a string\n");
                        printf("     on line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                  else
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
        "Fatal error:  second argument to TRANSLATE is other than a string\n");
                      printf("     on line %ld, column %ld.\n",source_line_num,
                       source_column_num);
                    }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
         "Fatal error:  first argument to TRANSLATE is other than a string\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                result_header_ptr=NULL;
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
                 function_name);
                printf("     line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
      return(result_header_ptr);
    }

static value_header_ptr true_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;

      if (queue_head != NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if (evaluate)
          {
            result_header_ptr=new_boolean_header_ptr();
            if (! fatal_error)
              *((*result_header_ptr).value_ptr.boolean)=TRUE;
          }
        else
          result_header_ptr=NULL;
      return(result_header_ptr);
    }

static value_header_ptr trunc_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      value_header_ptr result_header_ptr;
      int              status;
      double           tem_real_1;
      double           tem_real_2;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'I')
              {
                result_header_ptr=new_integer_header_ptr();
                if (! fatal_error)
                  *((*result_header_ptr).value_ptr.integer)
                   =*((*((*queue_head).argument_header_ptr)).
                   value_ptr.integer);
              }
            else
              if ((*((*queue_head).argument_header_ptr)).type == 'R')
                {
                  tem_real_1=*((*((*queue_head).argument_header_ptr)).
                   value_ptr.real);
                  if (tem_real_1 == 0.0)
                    {
                      result_header_ptr=new_integer_header_ptr();
                      if (! fatal_error)
                        *((*result_header_ptr).value_ptr.integer)=(long) 0;
                    }
                  else
                    {
                      tem_real_2=log(fabs(tem_real_1))/log(2.0);
                      if (tem_real_2 >= 31.0)
                        {
                          fatal_error=TRUE;
                          result_header_ptr=NULL;
                          printf(
                   "Fatal error:  magnitude of argument to TRUNC too large\n");
                          printf("     on line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        {
                          result_header_ptr=new_integer_header_ptr();
                          if (! fatal_error)
                            *((*result_header_ptr).value_ptr.integer)
                             =(long) tem_real_1;
                        }
                    }
                }
              else
                if ((*((*queue_head).argument_header_ptr)).type == 'B')
                  {
                    result_header_ptr=new_integer_header_ptr();
                    if (! fatal_error)
                      {
                        if (*((*((*queue_head).argument_header_ptr)).
                         value_ptr.boolean))
                          *((*result_header_ptr).value_ptr.integer)=1;
                        else
                          *((*result_header_ptr).value_ptr.integer)=0;
                      }
                  }
                else
                  if ((*((*queue_head).argument_header_ptr)).type == 'S')
                    {
                      result_header_ptr=new_integer_header_ptr();
                      if (! fatal_error)
                        {
                          status=sscanf((char *)
                           (*((*((*queue_head).argument_header_ptr)).
                           value_ptr.string)).value,"%I",
                           (*result_header_ptr).value_ptr.integer);
                          if ((status == EOF) || (status == 0))
                            {
                              fatal_error=TRUE;
                              free_value(result_header_ptr);
                              result_header_ptr=NULL;
                              printf(
                   "Fatal error:  argument to TRUNC cannot be converted on\n");
                              printf("     line %ld, column %ld.\n",
                               source_line_num,source_column_num);
                            }
                        }
                    }
                  else
                    {
                      fatal_error=TRUE;
                      result_header_ptr=NULL;
                      printf(
 "Fatal error:  argument to TRUNC is other than Boolean, number, or string\n");
                      printf("     on line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr upper_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      register int     char_index;
      unsigned char    *char_ptr;
      value_header_ptr result_header_ptr;
      int              string_length;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          if (evaluate)
            if ((*((*queue_head).argument_header_ptr)).type == 'S')
              {
                result_header_ptr=new_string_header_ptr((unsigned)
                 (*((*((*queue_head).argument_header_ptr)).value_ptr.string)).
                 length);
                if (! fatal_error)
                  {
                    pli_strcpy((*result_header_ptr).value_ptr.string,
                     (*((*queue_head).argument_header_ptr)).value_ptr.string);
                    char_ptr=(*((*result_header_ptr).value_ptr.string)).value;
                    string_length
                     =(*((*result_header_ptr).value_ptr.string)).length;
                    for (char_index=0; char_index < string_length;
                     char_index++)
                      {
                        *char_ptr=(unsigned char) toupper((int) *char_ptr);
                        char_ptr++;
                      }
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                 "Fatal error:  argument to UPPER is other than a string\n");
                printf("     on line %ld, column %ld.\n",source_line_num,
                 source_column_num);
              }
          else
            result_header_ptr=NULL;
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr verify_header_ptr(queue_head,function_name,evaluate)
  queue_node_ptr queue_head;
  char           *function_name;
  int            evaluate;
    {
      long             char_index;
      int              char_okay;
      unsigned char    *char_ptr;
      value_header_ptr result_header_ptr;
      unsigned         rule_index;
      unsigned         rule_length;
      unsigned char    *rule_ptr;
      long             string_length;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
          "Fatal error:  argument to function \"%s\" is missing on\n",
           function_name);
          printf("     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf(
             "Fatal error:  argument to function \"%s\" is missing on\n",
             function_name);
            printf("     line %ld, column %ld.\n",source_line_num,
             source_column_num);
          }
        else
          if ((*((*queue_head).next)).next == NULL)
            if (evaluate)
              if ((*((*queue_head).argument_header_ptr)).type == 'S')
                if ((*((*((*queue_head).next)).argument_header_ptr)).type
                 == 'S')
                  {
                    result_header_ptr=new_integer_header_ptr();
                    if (! fatal_error)
                      {
                        char_ptr=(*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).value;
                        string_length
                         =(long) (*((*((*queue_head).argument_header_ptr)).
                         value_ptr.string)).length;
                        char_index=(long) 1;
                        char_okay=TRUE;
                        while ((char_index <= string_length)
                        &&     (char_okay))
                          {
                            char_okay=FALSE;
                            rule_ptr
                             =(*((*((*((*queue_head).next)).
                             argument_header_ptr)).value_ptr.string)).value;
                            rule_length
                             =(unsigned) (*((*((*((*queue_head).next)).
                             argument_header_ptr)).value_ptr.string)).length;
                            rule_index=(unsigned) 1;
                            while ((! char_okay)
                            &&     (rule_index <= rule_length))
                              if (*rule_ptr == *char_ptr)
                                char_okay=TRUE;
                              else
                                {
                                  rule_ptr++;
                                  rule_index++;
                                }
                            if (char_okay)
                              {
                                char_ptr++;
                                char_index++;
                              }
                          }
                        if (char_okay)
                          *((*result_header_ptr).value_ptr.integer)=(long) 0;
                        else
                          *((*result_header_ptr).value_ptr.integer)
                           =char_index;
                      }
                  }
                else
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
           "Fatal error:  second argument to VERIFY is other than a string\n");
                    printf("     on line %ld, column %ld.\n",source_line_num,
                     source_column_num);
                  }
              else
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
            "Fatal error:  first argument to VERIFY is other than a string\n");
                  printf("     on line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            else
              result_header_ptr=NULL;
          else
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf(
          "Fatal error:  extraneous argument supplied to function \"%s\" on\n",
               function_name);
              printf("     line %ld, column %ld.\n",source_line_num,
               source_column_num);
            }
      return(result_header_ptr);
    }

static value_header_ptr function_header_ptr(evaluate)
  int evaluate;
    {
      queue_node_ptr   new_queue_head;
      queue_node_ptr   new_queue_tail;
      queue_node_ptr   queue_head;
      queue_node_ptr   queue_tail;
      value_header_ptr result_header_ptr;
      char             function_name [256];

      get_token();
      strcpy(function_name,source_token);
      queue_head=NULL;
      queue_tail=NULL;
      if (source_char == '(')
        {
          get_token();
          if ((queue_head=(queue_node_ptr)
           malloc((unsigned) sizeof(struct queue_node)))
           == NULL)
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf("Fatal error:  out of memory at line %ld, column %ld.\n",
               source_line_num,source_column_num);
            }
          else
            {
              queue_tail=queue_head;
              (*queue_head).next=NULL;
              (*queue_head).argument_header_ptr
               =interpret_expression(evaluate);
            }
          if (! fatal_error)
            get_token();
          while ((! fatal_error)
          &&     (! source_eof)
          &&     (source_token[0] != ')'))
            {
              if ((new_queue_tail=(queue_node_ptr)
               malloc((unsigned) sizeof(struct queue_node)))
               == NULL)
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
                   "Fatal error:  out of memory at line %ld, column %ld.\n",
                   source_line_num,source_column_num);
                }
              else
                {
                  (*new_queue_tail).next=NULL;
                  (*queue_tail).next=new_queue_tail;
                  queue_tail=new_queue_tail;
                  (*new_queue_tail).argument_header_ptr
                   =interpret_expression(evaluate);
                }
              if (! fatal_error)
                get_token();
            }
          if (! fatal_error)
            {
              if (source_token[0] != ')')
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
   "Fatal error:  file ends before arguments to function \"%s\" completed.\n",
                   function_name);
                }
            }
        }
      if (! fatal_error)
        {
          result_header_ptr
           =variable_header_ptr(function_name,evaluate,queue_head);
          if (! fatal_error)
            {
              if (result_header_ptr == NULL)
                {
                  if      (strcmp(function_name,"ABS") == 0)
                    result_header_ptr
                     =abs_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"ATAN") == 0)
                    result_header_ptr
                     =atan_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"CHAR") == 0)
                    result_header_ptr
                     =char_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"COS") == 0)
                    result_header_ptr
                     =cos_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"DATE") == 0)
                    result_header_ptr
                     =date_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"ENDFILE") == 0)
                    result_header_ptr
                     =endfile_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"EXEC") == 0)
                    result_header_ptr
                     =exec_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"EXP") == 0)
                    result_header_ptr
                     =exp_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"FALSE") == 0)
                    result_header_ptr
                     =false_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"FLOAT") == 0)
                    result_header_ptr
                     =float_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"GETCHAR") == 0)
                    result_header_ptr
                     =getchar_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"GETINT") == 0)
                    result_header_ptr
                     =getint_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"GETREAL") == 0)
                    result_header_ptr
                     =getreal_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"GETSTRING") == 0)
                    result_header_ptr
                     =getstring_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"INDEX") == 0)
                    result_header_ptr
                     =index_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"LENGTH") == 0)
                    result_header_ptr
                     =length_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"LINENO") == 0)
                    result_header_ptr
                     =lineno_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"LOG") == 0)
                    result_header_ptr
                     =log_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"MOD") == 0)
                    result_header_ptr
                     =mod_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"OPEN") == 0)
                    result_header_ptr
                     =open_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"ORD") == 0)
                    result_header_ptr
                     =ord_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"PI") == 0)
                    result_header_ptr
                     =pi_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"PLIRETV") == 0)
                    result_header_ptr
                     =pliretv_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"REPEAT") == 0)
                    result_header_ptr
                     =repeat_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"SIN") == 0)
                    result_header_ptr
                     =sin_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"SQR") == 0)
                    result_header_ptr
                     =sqr_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"SQRT") == 0)
                    result_header_ptr
                     =sqrt_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"STR") == 0)
                    result_header_ptr
                     =str_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"SUBSTR") == 0)
                    result_header_ptr
                     =substr_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"SYSIN") == 0)
                    result_header_ptr
                     =sysin_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"SYSPRINT") == 0)
                    result_header_ptr
                     =sysprint_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"TIME") == 0)
                    result_header_ptr
                     =time_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"TRANSLATE") == 0)
                    result_header_ptr
                     =translate_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"TRUE") == 0)
                    result_header_ptr
                     =true_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"TRUNC") == 0)
                    result_header_ptr
                     =trunc_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"UPPER") == 0)
                    result_header_ptr
                     =upper_header_ptr(queue_head,function_name,evaluate);
                  else if (strcmp(function_name,"VERIFY") == 0)
                    result_header_ptr
                     =verify_header_ptr(queue_head,function_name,evaluate);
                  else
                    {
                      if (evaluate)
                        {
                          fatal_error=TRUE;
                          printf(
                 "Fatal error:  the function \"%s\" on line %ld, column %ld\n",
                           function_name,source_line_num,source_column_num);
                          printf("     is unknown.\n");
                        }
                    }
               }
            }
        }
      while (queue_head != NULL)
        {
          new_queue_head=(*queue_head).next;
          free_value((*queue_head).argument_header_ptr);
          free((char *) queue_head);
          queue_head=new_queue_head;
        }
      return(result_header_ptr);
    }

static value_header_ptr factor_header_ptr(evaluate)
  int evaluate;
    {
      value_header_ptr result_header_ptr;

      while ((source_char == ' ')
      &&     (! source_eof))
        get_source_char();
      if (source_eof)
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  end of file encountered where factor expected.\n");
        }
      else
        {
          switch (source_char)
            {
              case '(':
                get_source_char();
                result_header_ptr=interpret_expression(evaluate);
                if (! fatal_error)
                  {
                    while ((source_char == ' ')
                    &&     (! source_eof))
                      get_source_char();
                    if (source_eof)
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
              "Fatal error:  end of file encountered where \"(\" expected.\n");
                      }
                    else
                      if (source_char == ')')
                        get_source_char();
                      else
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
   "Fatal error:  expression not followed by \"(\" on line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                  }
                break;
              case '!':
                get_source_char();
                result_header_ptr=factor_header_ptr(evaluate);
                if (! fatal_error)
                  {
                    if (evaluate)
                      if ((*result_header_ptr).type == 'B')
                        *((*result_header_ptr).value_ptr.boolean)
                         =! (*((*result_header_ptr).value_ptr.boolean));
                      else
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
       "Fatal error:  other than a boolean negated at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                    else
                      result_header_ptr=NULL;
                  }
                break;
              case '\'':
                result_header_ptr=string_header_ptr(evaluate);
                break;
              case '0':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                result_header_ptr=unsigned_number_header_ptr(evaluate);
                break;
              default:
               result_header_ptr=function_header_ptr(evaluate);
               break;
            }
        }
      return(result_header_ptr);
    }

static value_header_ptr and_factors(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      if (((*left_header_ptr).type == 'B')
      &&  ((*right_header_ptr).type == 'B'))
        {
          result_header_ptr=new_boolean_header_ptr();
          if (! fatal_error)
            *((*result_header_ptr).value_ptr.boolean)
             =(*((*left_header_ptr).value_ptr.boolean))
             && (*((*right_header_ptr).value_ptr.boolean));
        }
      else
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  attempt to \"and\" other than two booleans\n");
          printf(
           "at line %ld, column %ld.\n",source_line_num,source_column_num);
        }
      return(result_header_ptr);
    }

static value_header_ptr divide_factors(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      double           left_value;
      value_header_ptr result_header_ptr;
      double           right_value;
      double           tem_real;

      if (((*left_header_ptr).type == 'I')
      &&  ((*right_header_ptr).type == 'I'))
        {
          if (*((*right_header_ptr).value_ptr.integer) == (long) 0)
            {
              fatal_error=TRUE;
              result_header_ptr=NULL;
              printf(
         "Fatal error:  division by zero attempted at line %ld, column %ld.\n",
               source_line_num,source_column_num);
            }
          else
            {
              result_header_ptr=new_integer_header_ptr();
              if (! fatal_error)
                {
                  *((*result_header_ptr).value_ptr.integer)
                   =(*((*left_header_ptr).value_ptr.integer))
                   /(*((*right_header_ptr).value_ptr.integer));
                }
            }
        }
      else
        if (((*left_header_ptr).type == 'R')
        &&  ((*right_header_ptr).type == 'R'))
          {
            if (*((*right_header_ptr).value_ptr.real) == 0.0)
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
         "Fatal error:  division by zero attempted at line %ld, column %ld.\n",
                 source_line_num,source_column_num);
              }
            else
              {
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  {
                    left_value=*((*left_header_ptr).value_ptr.real);
                    right_value=*((*right_header_ptr).value_ptr.real);
                    if (left_value == 0.0)
                      tem_real=0.0;
                    else
                      tem_real
                       =(log(fabs(left_value))-log(fabs(right_value)))
                       /log(10.0);
                    if (tem_real < -37.0)
                      *((*result_header_ptr).value_ptr.real)=0.0;
                    else
                      if (tem_real > 37.0)
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
      "Fatal error:  overflow detected in division at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_value/right_value;
                  }
              }
          }
        else
          if (((*left_header_ptr).type == 'I')
          &&  ((*right_header_ptr).type == 'R'))
            {
              if (*((*right_header_ptr).value_ptr.real) == 0.0)
                {
                  fatal_error=TRUE;
                  result_header_ptr=NULL;
                  printf(
         "Fatal error:  division by zero attempted at line %ld, column %ld.\n",
                   source_line_num,source_column_num);
                }
              else
                {
                  result_header_ptr=new_real_header_ptr();
                  if (! fatal_error)
                    {
                      left_value
                       =(float) *((*left_header_ptr).value_ptr.integer);
                      right_value=*((*right_header_ptr).value_ptr.real);
                      if (left_value == 0.0)
                        tem_real=0.0;
                      else
                        tem_real
                         =(log(fabs(left_value))-log(fabs(right_value)))
                         /log(10.0);
                      if (tem_real < -37.0)
                        *((*result_header_ptr).value_ptr.real)=0.0;
                      else
                        if (tem_real > 37.0)
                          {
                            fatal_error=TRUE;
                            free_value(result_header_ptr);
                            result_header_ptr=NULL;
                            printf(
      "Fatal error:  overflow detected in division at line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                        else
                          *((*result_header_ptr).value_ptr.real)
                           =left_value/right_value;
                    }
                }
            }
          else
            if (((*left_header_ptr).type == 'R')
            &&  ((*right_header_ptr).type == 'I'))
              {
                if (*((*right_header_ptr).value_ptr.integer) == 0)
                  {
                    fatal_error=TRUE;
                    result_header_ptr=NULL;
                    printf(
         "Fatal error:  division by zero attempted at line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
                else
                  {
                    result_header_ptr=new_real_header_ptr();
                    if (! fatal_error)
                      {
                        left_value=*((*left_header_ptr).value_ptr.real);
                        right_value
                         =(float) *((*right_header_ptr).value_ptr.integer);
                        if (left_value == 0.0)
                          tem_real=0.0;
                        else
                          tem_real
                           =(log(fabs(left_value))-log(fabs(right_value)))
                           /log(10.0);
                        if (tem_real < -37.0)
                          *((*result_header_ptr).value_ptr.real)=0.0;
                        else
                          if (tem_real > 37.0)
                            {
                              fatal_error=TRUE;
                              free_value(result_header_ptr);
                              result_header_ptr=NULL;
                              printf(
      "Fatal error:  overflow detected in division at line %ld, column %ld.\n",
                               source_line_num,source_column_num);
                            }
                          else
                            *((*result_header_ptr).value_ptr.real)
                             =left_value/right_value;
                      }
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                "Fatal error:  attempt to divide other than two numbers at\n");
                printf(
                 "line %ld, column %ld.\n",source_line_num,source_column_num);
              }
      return(result_header_ptr);
    }

static value_header_ptr multiply_factors(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      long             left_integer_value;
      double           left_real_value;
      value_header_ptr result_header_ptr;
      long             right_integer_value;
      double           right_real_value;
      double           tem_real;

      if (((*left_header_ptr).type == 'I')
      &&  ((*right_header_ptr).type == 'I'))
        {
          result_header_ptr=new_integer_header_ptr();
          if (! fatal_error)
            {
              left_integer_value=*((*left_header_ptr).value_ptr.integer);
              right_integer_value=*((*right_header_ptr).value_ptr.integer);
              if ((left_integer_value == 0) || (right_integer_value == 0))
                tem_real=0.0;
              else
                {
                  left_real_value=(float) left_integer_value;
                  right_real_value=(float) right_integer_value;
                  tem_real
                   =(log(fabs(left_real_value))+log(fabs(right_real_value)))
                   /log(2.0);
                }
              if (tem_real >= 31.0)
                {
                  fatal_error=TRUE;
                  free_value(result_header_ptr);
                  result_header_ptr=NULL;
                  printf(
"Fatal error:  overflow detected in multiplication at line %ld, column %ld.\n",
                   source_line_num,source_column_num);
                }
              else
                *((*result_header_ptr).value_ptr.integer)
                 =left_integer_value*right_integer_value;
            }
        }
      else
        if (((*left_header_ptr).type == 'R')
        &&  ((*right_header_ptr).type == 'R'))
          {
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
              {
                left_real_value=*((*left_header_ptr).value_ptr.real);
                right_real_value=*((*right_header_ptr).value_ptr.real);
                if ((left_real_value == 0.0) || (right_real_value == 0.0))
                  tem_real=0.0;
                else
                  tem_real
                   =(log(fabs(left_real_value))+log(fabs(right_real_value)))
                   /log(10.0);
                if (tem_real < -37.0)
                  *((*result_header_ptr).value_ptr.real)=0.0;
                else
                  if (tem_real > 37.0)
                    {
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
"Fatal error:  overflow detected in multiplication at line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
                  else
                    *((*result_header_ptr).value_ptr.real)
                     =left_real_value*right_real_value;
              }
          }
        else
          if (((*left_header_ptr).type == 'I')
          &&  ((*right_header_ptr).type == 'R'))
            {
              result_header_ptr=new_real_header_ptr();
              if (! fatal_error)
                {
                  left_real_value
                   =(float) *((*left_header_ptr).value_ptr.integer);
                  right_real_value=*((*right_header_ptr).value_ptr.real);
                  if ((left_real_value == 0.0) || (right_real_value == 0.0))
                    tem_real=0.0;
                  else
                    tem_real
                     =(log(fabs(left_real_value))+log(fabs(right_real_value)))
                     /log(10.0);
                  if (tem_real < -37.0)
                    *((*result_header_ptr).value_ptr.real)=0.0;
                  else
                    if (tem_real > 37.0)
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
"Fatal error:  overflow detected in multiplication at line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                    else
                      *((*result_header_ptr).value_ptr.real)
                       =left_real_value*right_real_value;
                }
            }
          else
            if (((*left_header_ptr).type == 'R')
            &&  ((*right_header_ptr).type == 'I'))
              {
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  {
                    left_real_value=*((*left_header_ptr).value_ptr.real);
                    right_real_value
                     =(float) *((*right_header_ptr).value_ptr.integer);
                    if ((left_real_value == 0.0) || (right_real_value == 0.0))
                      tem_real=0.0;
                    else
                      tem_real
                       =(log(fabs(left_real_value))
                       +log(fabs(right_real_value)))
                       /log(10.0);
                    if (tem_real < -37.0)
                      *((*result_header_ptr).value_ptr.real)=0.0;
                    else
                      if (tem_real > 37.0)
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
"Fatal error:  overflow detected in multiplication at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value*right_real_value;
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
              "Fatal error:  attempt to multiply other than two numbers at\n");
                printf(
                 "line %ld, column %ld.\n",source_line_num,source_column_num);
              }
      return(result_header_ptr);
    }

static void get_factor_operator(operator)
  char *operator;
    {
      while ((source_char == ' ')
      &&     (! source_eof))
        get_source_char();
      switch (source_char)
        {
          case '*':
            *operator=source_char;
            get_source_char();
            break;
          case '/':
            *operator=source_char;
            get_source_char();
            break;
          case '&':
            *operator=source_char;
            get_source_char();
            break;
          default:
            *operator='\0';
            break;
        }
      return;
    }

static value_header_ptr term_header_ptr(evaluate)
  int evaluate;
    {
      value_header_ptr left_header_ptr;
      char             operator;
      int              operator_found;
      value_header_ptr result_header_ptr;
      value_header_ptr right_header_ptr;

      while ((source_char == ' ')
      &&     (! source_eof))
        get_source_char();
      if (source_char == ' ')
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  end of file encountered where term expected.\n");
        }
      else
        {
          result_header_ptr=factor_header_ptr(evaluate);
          operator_found=TRUE;
          while ((! fatal_error)
          &&     (operator_found))
            {
              get_factor_operator(&operator);
              if ((operator != '*')
              &&  (operator != '/')
              &&  (operator != '&'))
                 operator_found=FALSE;
              else
                {
                  right_header_ptr=factor_header_ptr(evaluate);
                  if (fatal_error)
                    {
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                    }
                  else
                    {
                      left_header_ptr=result_header_ptr;
                      if (evaluate)
                        {
                          switch (operator)
                            {
                              case '*':
                                result_header_ptr=multiply_factors(
                                 left_header_ptr,right_header_ptr);
                                break;
                              case '/':
                                result_header_ptr=divide_factors(
                                 left_header_ptr,right_header_ptr);
                                break;
                              default:
                                result_header_ptr=and_factors(
                                 left_header_ptr,right_header_ptr);
                                break;
                            }
                          free_value(left_header_ptr);
                          free_value(right_header_ptr);
                        }
                      else
                        result_header_ptr=NULL;
                    }
                }
            }
        }
      return(result_header_ptr);
    }

static value_header_ptr concatenate_terms(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      register int     char_index;
      unsigned char    *char_ptr;
      unsigned char    *result_char_ptr;
      value_header_ptr result_header_ptr;
      unsigned         string_length;

      if (((*left_header_ptr).type == 'S')
      &&  ((*right_header_ptr).type == 'S'))
        {
          result_header_ptr=new_string_header_ptr((unsigned)
           (*((*left_header_ptr).value_ptr.string)).length
           +(unsigned) (*((*right_header_ptr).value_ptr.string)).length);
          if (! fatal_error)
            {
              result_char_ptr=(*((*result_header_ptr).value_ptr.string)).value;
              char_ptr=(*((*left_header_ptr).value_ptr.string)).value;
              string_length=(*((*left_header_ptr).value_ptr.string)).length;
              for (char_index=0; char_index < string_length; char_index++)
                {
                  *result_char_ptr=*char_ptr;
                  result_char_ptr++;
                  char_ptr++;
                }
              char_ptr=(*((*right_header_ptr).value_ptr.string)).value;
              string_length=(*((*right_header_ptr).value_ptr.string)).length;
              for (char_index=0; char_index < string_length; char_index++)
                {
                  *result_char_ptr=*char_ptr;
                  result_char_ptr++;
                  char_ptr++;
                }
              *result_char_ptr=(unsigned char) '\0';
            }
        }
      else
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  attempt to concatenate other than two strings\n");
          printf(
           "at line %ld, column %ld.\n",source_line_num,source_column_num);
        }
      return(result_header_ptr);
    }

static value_header_ptr add_terms(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      long             left_integer_value;
      double           left_real_value;
      value_header_ptr result_header_ptr;
      long             right_integer_value;
      double           right_real_value;

      if (((*left_header_ptr).type == 'I')
      &&  ((*right_header_ptr).type == 'I'))
        {
          result_header_ptr=new_integer_header_ptr();
          if (! fatal_error)
            {
              left_integer_value=*((*left_header_ptr).value_ptr.integer);
              right_integer_value=*((*right_header_ptr).value_ptr.integer);
              if ((left_integer_value > 0) && (right_integer_value > 0))
                if (left_integer_value
                 > ((long) 0x7fffffff - right_integer_value))
                  {
                    fatal_error=TRUE;
                    free_value(result_header_ptr);
                    result_header_ptr=NULL;
                    printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
                else
                  *((*result_header_ptr).value_ptr.integer)
                   =left_integer_value+right_integer_value;
              else
                if ((left_integer_value < 0) && (right_integer_value < 0))
                  if (left_integer_value
                   < (-((long) 0x7fffffff) - right_integer_value))
                    {
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
                  else
                    *((*result_header_ptr).value_ptr.integer)
                     =left_integer_value+right_integer_value;
                else
                  *((*result_header_ptr).value_ptr.integer)
                   =left_integer_value+right_integer_value;
            }
        }
      else
        if (((*left_header_ptr).type == 'R')
        &&  ((*right_header_ptr).type == 'R'))
          {
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
              {
                left_real_value=*((*left_header_ptr).value_ptr.real);
                right_real_value=*((*right_header_ptr).value_ptr.real);
                if ((left_real_value > 0.0) && (right_real_value > 0.0))
                  if (left_real_value > (1.0E37 - right_real_value))
                    {
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
                  else
                    *((*result_header_ptr).value_ptr.real)
                     =left_real_value+right_real_value;
                else
                  if ((left_real_value < 0.0) && (right_real_value < 0.0))
                    if (left_real_value < (-1.0E37 - right_real_value))
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                    else
                      *((*result_header_ptr).value_ptr.real)
                       =left_real_value+right_real_value;
                  else
                    *((*result_header_ptr).value_ptr.real)
                     =left_real_value+right_real_value;
              }
          }
        else
          if (((*left_header_ptr).type == 'I')
          &&  ((*right_header_ptr).type == 'R'))
            {
              result_header_ptr=new_real_header_ptr();
              if (! fatal_error)
                {
                  left_real_value=(double)
                   *((*left_header_ptr).value_ptr.integer);
                  right_real_value=*((*right_header_ptr).value_ptr.real);
                  if ((left_real_value > 0.0) && (right_real_value > 0.0))
                    if (left_real_value > (1.0E37 - right_real_value))
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                    else
                      *((*result_header_ptr).value_ptr.real)
                       =left_real_value+right_real_value;
                  else
                    if ((left_real_value < 0.0) && (right_real_value < 0.0))
                      if (left_real_value < (-1.0E37 - right_real_value))
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value+right_real_value;
                   else
                     *((*result_header_ptr).value_ptr.real)
                      =left_real_value+right_real_value;
                }
            }
          else
            if (((*left_header_ptr).type == 'R')
            &&  ((*right_header_ptr).type == 'I'))
              {
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  {
                    left_real_value=*((*left_header_ptr).value_ptr.real);
                    right_real_value=*((*right_header_ptr).value_ptr.real);
                    if ((left_real_value > 0.0)
                    && (right_real_value > 0.0))
                      if (left_real_value > (1.0E37 - right_real_value))
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value+right_real_value;
                    else
                      if ((left_real_value < 0.0)
                      &&  (right_real_value < 0.0))
                        if (left_real_value < (-1.0E37 - right_real_value))
                          {
                            fatal_error=TRUE;
                            free_value(result_header_ptr);
                            result_header_ptr=NULL;
                            printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                        else
                          *((*result_header_ptr).value_ptr.real)
                           =left_real_value+right_real_value;
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value+right_real_value;
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                 "Fatal error:  attempt to add other than two numbers at\n");
                printf(
                 "line %ld, column %ld.\n",source_line_num,source_column_num);
              }
      return(result_header_ptr);
    }

static value_header_ptr subtract_terms(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      long             left_integer_value;
      double           left_real_value;
      value_header_ptr result_header_ptr;
      long             right_integer_value;
      double           right_real_value;

      if (((*left_header_ptr).type == 'I')
      &&  ((*right_header_ptr).type == 'I'))
        {
          result_header_ptr=new_integer_header_ptr();
          if (! fatal_error)
            {
              left_integer_value=*((*left_header_ptr).value_ptr.integer);
              right_integer_value=*((*right_header_ptr).value_ptr.integer);
              if ((left_integer_value < 0) && (right_integer_value > 0))
                if (left_integer_value
                 < (right_integer_value-((long) 0x7fffffff)))
                  {
                    fatal_error=TRUE;
                    free_value(result_header_ptr);
                    result_header_ptr=NULL;
                    printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
                else
                  *((*result_header_ptr).value_ptr.integer)
                   =left_integer_value-right_integer_value;
              else
                if ((left_integer_value > 0) && (right_integer_value < 0))
                  if (left_integer_value
                   > (right_integer_value+(long) 0x7fffffff))
                    {
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
                  else
                    *((*result_header_ptr).value_ptr.integer)
                     =left_integer_value-right_integer_value;
                else
                  *((*result_header_ptr).value_ptr.integer)
                   =left_integer_value-right_integer_value;
            }
        }
      else
        if (((*left_header_ptr).type == 'R')
        &&  ((*right_header_ptr).type == 'R'))
          {
            result_header_ptr=new_real_header_ptr();
            if (! fatal_error)
              {
                left_real_value=*((*left_header_ptr).value_ptr.real);
                right_real_value=*((*right_header_ptr).value_ptr.real);
                if ((left_real_value < 0.0) && (right_real_value > 0.0))
                  if (left_real_value < (right_real_value-1.0E37))
                    {
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
                  else
                    *((*result_header_ptr).value_ptr.real)
                     =left_real_value-right_real_value;
                else
                  if ((left_real_value > 0.0) && (right_real_value < 0.0))
                    if (left_real_value > (right_real_value+1.0E37))
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                    else
                      *((*result_header_ptr).value_ptr.real)
                       =left_real_value-right_real_value;
                  else
                    *((*result_header_ptr).value_ptr.real)
                     =left_real_value-right_real_value;
              }
          }
        else
          if (((*left_header_ptr).type == 'I')
          &&  ((*right_header_ptr).type == 'R'))
            {
              result_header_ptr=new_real_header_ptr();
              if (! fatal_error)
                {
                  left_real_value=(double)
                   *((*left_header_ptr).value_ptr.integer);
                  right_real_value=*((*right_header_ptr).value_ptr.real);
                  if ((left_real_value < 0.0) && (right_real_value > 0.0))
                    if (left_real_value < (right_real_value-1.0E37))
                      {
                        fatal_error=TRUE;
                        free_value(result_header_ptr);
                        result_header_ptr=NULL;
                        printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                         source_line_num,source_column_num);
                      }
                    else
                      *((*result_header_ptr).value_ptr.real)
                       =left_real_value-right_real_value;
                  else
                    if ((left_real_value > 0.0) && (right_real_value < 0.0))
                      if (left_real_value > (right_real_value+1.0E37))
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value-right_real_value;
                   else
                     *((*result_header_ptr).value_ptr.real)
                      =left_real_value-right_real_value;
                }
            }
          else
            if (((*left_header_ptr).type == 'R')
            &&  ((*right_header_ptr).type == 'I'))
              {
                result_header_ptr=new_real_header_ptr();
                if (! fatal_error)
                  {
                    left_real_value=*((*left_header_ptr).value_ptr.real);
                    right_real_value=*((*right_header_ptr).value_ptr.real);
                    if ((left_real_value < 0.0)
                    && (right_real_value > 0.0))
                      if (left_real_value < (right_real_value-1.0E37))
                        {
                          fatal_error=TRUE;
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                          printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                           source_line_num,source_column_num);
                        }
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value-right_real_value;
                    else
                      if ((left_real_value > 0.0)
                      &&  (right_real_value < 0.0))
                        if (left_real_value > (right_real_value+1.0E37))
                          {
                            fatal_error=TRUE;
                            free_value(result_header_ptr);
                            result_header_ptr=NULL;
                            printf(
      "Fatal error:  overflow detected in addition at line %ld, column %ld.\n",
                             source_line_num,source_column_num);
                          }
                        else
                          *((*result_header_ptr).value_ptr.real)
                           =left_real_value-right_real_value;
                      else
                        *((*result_header_ptr).value_ptr.real)
                         =left_real_value-right_real_value;
                  }
              }
            else
              {
                fatal_error=TRUE;
                result_header_ptr=NULL;
                printf(
                 "Fatal error:  attempt to add other than two numbers at\n");
                printf(
                 "line %ld, column %ld.\n",source_line_num,source_column_num);
              }
      return(result_header_ptr);
    }

static value_header_ptr or_terms(left_header_ptr,right_header_ptr)
  value_header_ptr left_header_ptr;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      if (((*left_header_ptr).type == 'B')
      &&  ((*right_header_ptr).type == 'B'))
        {
          result_header_ptr=new_boolean_header_ptr();
          if (! fatal_error)
            *((*result_header_ptr).value_ptr.boolean)
             =(*((*left_header_ptr).value_ptr.boolean))
             || (*((*right_header_ptr).value_ptr.boolean));
        }
      else
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
           "Fatal error:  attempt to \"or\" other than two booleans\n");
          printf(
           "at line %ld, column %ld.\n",source_line_num,source_column_num);
        }
      return(result_header_ptr);
    }

static void get_term_operator(operator)
  char *operator;
    {
      while ((source_char == ' ')
      &&     (! source_eof))
        get_source_char();
      switch (source_char)
        {
          case '+':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            break;
          case '-':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            break;
          case '|':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            if (source_char == '|')
              {
                operator[1]='|';
                operator[2]='\0';
                get_source_char();
              }
            break;
          default:
            operator[0]='\0';
            break;
        }
      return;
    }

static value_header_ptr simple_expression_header_ptr(evaluate)
  int evaluate;
    {
      char             leading_sign;
      value_header_ptr left_header_ptr;
      char             operator [3];
      int              operator_found;
      value_header_ptr result_header_ptr;
      value_header_ptr right_header_ptr;

      while ((source_char == ' ')
      &&     (! source_eof))
        get_source_char();
      if (source_char == ' ')
        {
          fatal_error=TRUE;
          result_header_ptr=NULL;
          printf(
  "Fatal error:  end of file encountered where simple expression expected.\n");
        }
      else
        {
          leading_sign=' ';
          if ((source_char == '+') || (source_char == '-'))
            {
              leading_sign=source_char;
              get_source_char();
            }
          result_header_ptr=term_header_ptr(evaluate);
          if (! fatal_error)
            {
              if ((evaluate) && (leading_sign != ' '))
                switch ((*result_header_ptr).type)
                  {
                    case 'I':
                      if (leading_sign == '-')
                        *((*result_header_ptr).value_ptr.integer)
                         =-(*((*result_header_ptr).value_ptr.integer));
                      break;
                    case 'R':
                      if (leading_sign == '-')
                        *((*result_header_ptr).value_ptr.real)
                         =-(*((*result_header_ptr).value_ptr.real));
                      break;
                    default:
                      fatal_error=TRUE;
                      free_value(result_header_ptr);
                      result_header_ptr=NULL;
                      printf(
                       "Fatal error:  sign applied to other than number at ");
                      printf(
                       "line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                      break;
                  }
              operator_found=TRUE;
              while ((! fatal_error)
              &&     (operator_found))
                {
                  get_term_operator(operator);
                  if ((strcmp(operator,"||") != 0)
                  &&  (strcmp(operator,"|") != 0)
                  &&  (strcmp(operator,"+") != 0)
                  &&  (strcmp(operator,"-") != 0))
                    operator_found=FALSE;
                  else
                    {
                      right_header_ptr=term_header_ptr(evaluate);
                      if (fatal_error)
                        {
                          free_value(result_header_ptr);
                          result_header_ptr=NULL;
                        }
                      else
                        {
                          left_header_ptr=result_header_ptr;
                          if (evaluate)
                            {
                              if (strcmp(operator,"||") == 0)
                                result_header_ptr=concatenate_terms(
                                 left_header_ptr,right_header_ptr);
                              else
                                switch (operator[0])
                                  {
                                    case '+':
                                      result_header_ptr=add_terms(
                                       left_header_ptr,right_header_ptr);
                                      break;
                                    case '-':
                                      result_header_ptr=subtract_terms(
                                       left_header_ptr,right_header_ptr);
                                      break;
                                    default:
                                      result_header_ptr=or_terms(
                                       left_header_ptr,right_header_ptr);
                                      break;
                                  }
                              free_value(left_header_ptr);
                              free_value(right_header_ptr);
                            }
                          else
                            result_header_ptr=NULL;
                        }
                    }
                }
            }
        }
      return(result_header_ptr);
    }

static value_header_ptr boolean_comparison(left_header_ptr,operator,
 right_header_ptr)
  value_header_ptr left_header_ptr;
  char             *operator;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      if (strcmp(operator,"!=") == 0)
        {
          result_header_ptr=new_boolean_header_ptr();
          if (! fatal_error)
            {
              if (*((*left_header_ptr).value_ptr.boolean)
               != *((*right_header_ptr).value_ptr.boolean))
                *((*result_header_ptr).value_ptr.boolean)=TRUE;
              else
                *((*result_header_ptr).value_ptr.boolean)=FALSE;
            }
        }
      else
        if (strcmp(operator,"=") == 0)
          {
            result_header_ptr=new_boolean_header_ptr();
            if (! fatal_error)
              {
                if (*((*left_header_ptr).value_ptr.boolean)
                 == *((*right_header_ptr).value_ptr.boolean))
                  *((*result_header_ptr).value_ptr.boolean)=TRUE;
                else
                  *((*result_header_ptr).value_ptr.boolean)=FALSE;
              }
          }
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf("Fatal error:  \"%s\" used to compare booleans at ",
             operator);
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr dataset_comparison(left_header_ptr,operator,
 right_header_ptr)
  value_header_ptr left_header_ptr;
  char             *operator;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      if (strcmp(operator,"!=") == 0)
        {
          result_header_ptr=new_boolean_header_ptr();
          if (! fatal_error)
            {
              if (*((*left_header_ptr).value_ptr.dataset)
               != *((*right_header_ptr).value_ptr.dataset))
                *((*result_header_ptr).value_ptr.boolean)=TRUE;
              else
                *((*result_header_ptr).value_ptr.boolean)=FALSE;
            }
        }
      else
        if (strcmp(operator,"=") == 0)
          {
            result_header_ptr=new_boolean_header_ptr();
            if (! fatal_error)
              {
                if (*((*left_header_ptr).value_ptr.dataset)
                 == *((*right_header_ptr).value_ptr.dataset))
                  *((*result_header_ptr).value_ptr.boolean)=TRUE;
                else
                  *((*result_header_ptr).value_ptr.boolean)=FALSE;
              }
          }
        else
          {
            fatal_error=TRUE;
            result_header_ptr=NULL;
            printf("Fatal error:  \"%s\" used to compare file pointers at ",
             operator);
            printf("line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
      return(result_header_ptr);
    }

static value_header_ptr integer_comparison(left_header_ptr,operator,
 right_header_ptr)
  value_header_ptr left_header_ptr;
  char             *operator;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      result_header_ptr=new_boolean_header_ptr();
      if (! fatal_error)
        {
          if (strcmp(operator,"<=") == 0)
            if (*((*left_header_ptr).value_ptr.integer)
             <= *((*right_header_ptr).value_ptr.integer))
              *((*result_header_ptr).value_ptr.boolean)=TRUE;
            else
              *((*result_header_ptr).value_ptr.boolean)=FALSE;
          else
            if (strcmp(operator,">=") == 0)
              if (*((*left_header_ptr).value_ptr.integer)
               >= *((*right_header_ptr).value_ptr.integer))
                *((*result_header_ptr).value_ptr.boolean)=TRUE;
              else
                *((*result_header_ptr).value_ptr.boolean)=FALSE;
            else
              if (strcmp(operator,"!=") == 0)
                if (*((*left_header_ptr).value_ptr.integer)
                 != *((*right_header_ptr).value_ptr.integer))
                  *((*result_header_ptr).value_ptr.boolean)=TRUE;
                else
                  *((*result_header_ptr).value_ptr.boolean)=FALSE;
              else
                if (strcmp(operator,">") == 0)
                  if (*((*left_header_ptr).value_ptr.integer)
                   > *((*right_header_ptr).value_ptr.integer))
                    *((*result_header_ptr).value_ptr.boolean)=TRUE;
                  else
                    *((*result_header_ptr).value_ptr.boolean)=FALSE;
                else
                  if (strcmp(operator,"<") == 0)
                    if (*((*left_header_ptr).value_ptr.integer)
                     < *((*right_header_ptr).value_ptr.integer))
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
                  else
                    if (*((*left_header_ptr).value_ptr.integer)
                     == *((*right_header_ptr).value_ptr.integer))
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
        }
      return(result_header_ptr);
    }


static value_header_ptr real_comparison(left_header_ptr,operator,
 right_header_ptr)
  value_header_ptr left_header_ptr;
  char             *operator;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      result_header_ptr=new_boolean_header_ptr();
      if (! fatal_error)
        {
          if (strcmp(operator,"<=") == 0)
            if (*((*left_header_ptr).value_ptr.real)
             <= *((*right_header_ptr).value_ptr.real))
              *((*result_header_ptr).value_ptr.boolean)=TRUE;
            else
              *((*result_header_ptr).value_ptr.boolean)=FALSE;
          else
            if (strcmp(operator,">=") == 0)
              if (*((*left_header_ptr).value_ptr.real)
               >= *((*right_header_ptr).value_ptr.real))
                *((*result_header_ptr).value_ptr.boolean)=TRUE;
              else
                *((*result_header_ptr).value_ptr.boolean)=FALSE;
            else
              if (strcmp(operator,"!=") == 0)
                if (*((*left_header_ptr).value_ptr.real)
                 != *((*right_header_ptr).value_ptr.real))
                  *((*result_header_ptr).value_ptr.boolean)=TRUE;
                else
                  *((*result_header_ptr).value_ptr.boolean)=FALSE;
              else
                if (strcmp(operator,">") == 0)
                  if (*((*left_header_ptr).value_ptr.real)
                   > *((*right_header_ptr).value_ptr.real))
                    *((*result_header_ptr).value_ptr.boolean)=TRUE;
                  else
                    *((*result_header_ptr).value_ptr.boolean)=FALSE;
                else
                  if (strcmp(operator,"<") == 0)
                    if (*((*left_header_ptr).value_ptr.real)
                     < *((*right_header_ptr).value_ptr.real))
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
                  else
                    if (*((*left_header_ptr).value_ptr.real)
                     == *((*right_header_ptr).value_ptr.real))
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
        }
      return(result_header_ptr);
    }


static value_header_ptr string_comparison(left_header_ptr,operator,
 right_header_ptr)
  value_header_ptr left_header_ptr;
  char             *operator;
  value_header_ptr right_header_ptr;
    {
      value_header_ptr result_header_ptr;

      result_header_ptr=new_boolean_header_ptr();
      if (! fatal_error)
        {
          if (strcmp(operator,"<=") == 0)
            if (pli_strcmp((*left_header_ptr).value_ptr.string,
             (*right_header_ptr).value_ptr.string) <= 0)
              *((*result_header_ptr).value_ptr.boolean)=TRUE;
            else
              *((*result_header_ptr).value_ptr.boolean)=FALSE;
          else
            if (strcmp(operator,">=") == 0)
              if (pli_strcmp((*left_header_ptr).value_ptr.string,
                (*right_header_ptr).value_ptr.string) >= 0)
                *((*result_header_ptr).value_ptr.boolean)=TRUE;
              else
                *((*result_header_ptr).value_ptr.boolean)=FALSE;
            else
              if (strcmp(operator,"!=") == 0)
                if (pli_strcmp((*left_header_ptr).value_ptr.string,
                 (*right_header_ptr).value_ptr.string) != 0)
                  *((*result_header_ptr).value_ptr.boolean)=TRUE;
                else
                  *((*result_header_ptr).value_ptr.boolean)=FALSE;
              else
                if (strcmp(operator,"<") == 0)
                  if (pli_strcmp((*left_header_ptr).value_ptr.string,
                   (*right_header_ptr).value_ptr.string) < 0)
                    *((*result_header_ptr).value_ptr.boolean)=TRUE;
                  else
                    *((*result_header_ptr).value_ptr.boolean)=FALSE;
                else
                  if (strcmp(operator,">") == 0)
                    if (pli_strcmp((*left_header_ptr).value_ptr.string,
                     (*right_header_ptr).value_ptr.string) > 0)
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
                  else
                    if (pli_strcmp((*left_header_ptr).value_ptr.string,
                     (*right_header_ptr).value_ptr.string) == 0)
                      *((*result_header_ptr).value_ptr.boolean)=TRUE;
                    else
                      *((*result_header_ptr).value_ptr.boolean)=FALSE;
        }
      return(result_header_ptr);
    }

static void get_comparison_operator(operator)
  char *operator;
    {
      while ((source_char == ' ')
      &&     (! source_eof))
        get_source_char();
      switch (source_char)
        {
          case '=':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            break;
          case '<':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            if (source_char == '=')
              {
                operator[1]='=';
                operator[2]='\0';
                get_source_char();
              }
            break;
          case '!':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            if (source_char == '=')
              {
                operator[1]='=';
                operator[2]='\0';
                get_source_char();
              }
            break;
          case '>':
            operator[0]=source_char;
            operator[1]='\0';
            get_source_char();
            if (source_char == '=')
              {
                operator[1]='=';
                operator[2]='\0';
                get_source_char();
              }
            break;
          default:
            operator[0]='\0';
            break;
        }
      return;
    }

static value_header_ptr interpret_expression(evaluate)
  int evaluate;
    {
      value_header_ptr left_header_ptr;
      char             operator [3];
      value_header_ptr result_header_ptr;
      value_header_ptr right_header_ptr;
      double           tem_real_1;

      left_header_ptr=simple_expression_header_ptr(evaluate);
      if (fatal_error)
        result_header_ptr=NULL;
      else
        {
          get_comparison_operator(operator);
          if ((strcmp(operator,"<=") != 0)
          &&  (strcmp(operator,">=") != 0)
          &&  (strcmp(operator,"!=") != 0)
          &&  (strcmp(operator,"<") != 0)
          &&  (strcmp(operator,">") != 0)
          &&  (strcmp(operator,"=") != 0))
             result_header_ptr=left_header_ptr;
          else
            {
              right_header_ptr=simple_expression_header_ptr(evaluate);
              if (fatal_error)
                {
                  free_value(left_header_ptr);
                  result_header_ptr=NULL;
                }
              else
                {
                  if (evaluate)
                    {
                      if ((*left_header_ptr).type
                       == (*right_header_ptr).type)
                        switch ((*left_header_ptr).type)
                          {
                            case 'B':
                              result_header_ptr=boolean_comparison(
                               left_header_ptr,operator,
                               right_header_ptr);
                              break;
                            case 'D':
                              result_header_ptr=dataset_comparison(
                               left_header_ptr,operator,
                               right_header_ptr);
                              break;
                            case 'I':
                              result_header_ptr=integer_comparison(
                               left_header_ptr,operator,
                               right_header_ptr);
                              break;
                            case 'R':
                              result_header_ptr=real_comparison(
                               left_header_ptr,operator,
                               right_header_ptr);
                              break;
                            default:
                              result_header_ptr=string_comparison(
                               left_header_ptr,operator,
                               right_header_ptr);
                              break;
                          }
                      else
                        if (((*left_header_ptr).type == 'I')
                        &&  ((*right_header_ptr).type == 'R'))
                          {
                            tem_real_1=(double)
                             *((*left_header_ptr).value_ptr.integer);
                            free((char *)
                             (*left_header_ptr).value_ptr.integer);
                            if (((*left_header_ptr).value_ptr.real
                             =(double *)
                             malloc((unsigned) sizeof(double))) == NULL)
                              {
                                fatal_error=TRUE;
                                result_header_ptr=NULL;
                                printf("Fatal error:  out of memory at ");
                                printf("line %ld, column %ld.\n",
                                 source_line_num,source_column_num);
                                free((char *) left_header_ptr);
                                free_value(right_header_ptr);
                              }
                            else
                              {
                                *((*left_header_ptr).value_ptr.real)
                                 =tem_real_1;
                                (*left_header_ptr).type='R';
                                result_header_ptr=real_comparison(
                                 left_header_ptr,operator,
                                 right_header_ptr);
                              }
                          }
                        else
                          if (((*left_header_ptr).type == 'R')
                          &&  ((*right_header_ptr).type == 'I'))
                            {
                              tem_real_1=(double)
                               *((*right_header_ptr).value_ptr.integer);
                              free((char *)
                               (*right_header_ptr).value_ptr.integer);
                              if (((*right_header_ptr).value_ptr.real
                               =(double *)
                               malloc((unsigned) sizeof(double))) == NULL)
                                {
                                  fatal_error=TRUE;
                                  result_header_ptr=NULL;
                                  printf("Fatal error:  out of memory ");
                                  printf("at line %ld, column %ld.\n",
                                   source_line_num,source_column_num);
                                  free((char *) right_header_ptr);
                                  free_value(left_header_ptr);
                                }
                              else
                                {
                                  *((*right_header_ptr).value_ptr.real)
                                   =tem_real_1;
                                  (*right_header_ptr).type='R';
                                  result_header_ptr=real_comparison(
                                   left_header_ptr,operator,
                                   right_header_ptr);
                                }
                            }
                          else
                            {
                              fatal_error=TRUE;
                              result_header_ptr=NULL;
                              printf("Fatal error:  comparands differ ");
                              printf("in type at line %ld, column %ld.\n",
                               source_line_num,source_column_num);
                              free_value(left_header_ptr);
                              free_value(right_header_ptr);
                            }
                      if (! fatal_error)
                        {
                          free_value(left_header_ptr);
                          free_value(right_header_ptr);
                        }
                    }
                  else
                    result_header_ptr=NULL;
                }
            }
        }
      return(result_header_ptr);
    }

static void interpret_do(evaluate)
  int evaluate;
    {
      int              condition_is_true;
      value_header_ptr expression_header_ptr;
      char             while_char;
      long             while_column_num;
      int              while_eof;
      long             while_page_num;
      int              while_page_index;
      long             while_line_num;

      get_token();
      if (source_token[0] == ';')
        {
          do
            {
              get_token();
              if (source_token[0] == ' ')
                {
                  fatal_error=TRUE;
                  printf("Fatal error:  file ends before \"END;\" ");
                  printf("corresponding to \"DO;\".\n");
                }
              else
                {
                  if (strcmp(source_token,"END") != 0)
                    interpret_statement(evaluate);
                }
            }
          while ((strcmp(source_token,"END") != 0)
          &&     (! fatal_error));
          if (! fatal_error)
            {
              get_token();
              if (source_token[0] == ' ')
                {
                  fatal_error=TRUE;
                  printf("Fatal error:  file ends where \";\" of \"END;\" ");
                  printf("expected.\n");
                }
              else
                {
                  if (source_token[0] != ';')
                    {
                      fatal_error=TRUE;
                      printf(
                     "Fatal error:  \";\" expected at line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                    }
                }
            }
        }
      else
        if (strcmp(source_token,"WHILE") == 0)
          {
            get_token();
            if (source_token[0] == ' ')
              {
                fatal_error=TRUE;
                printf(
           "Fatal error:  file ends where \"(\" of \"DO WHILE(\" expected.\n");
              }
            else
              if (source_token[0] == '(')
                {
                  while_page_index=page_index;
                  while_page_num=page_num;
                  while_char=source_char;
                  while_column_num=source_column_num;
                  while_eof=source_eof;
                  while_line_num=source_line_num;
                  do
                    {
                      if (while_page_num != page_num)
                        {
                          source_index=while_page_num;
                          source_index*=(long) page_size;
                          fseek(source_file,source_index,SEEK_SET);
                          fread(&page[0],1,page_size,source_file);
                        }
                      page_num=while_page_num;
                      page_index=while_page_index;
                      source_char=while_char;
                      source_column_num=while_column_num;
                      source_eof=while_eof;
                      source_line_num=while_line_num;
                      expression_header_ptr=interpret_expression(evaluate);
                      if (! fatal_error)
                        {
                          if ((! evaluate)
                          ||  ((*expression_header_ptr).type == 'B'))
                            {
                              get_token();
                              if (source_token[0] == ')')
                                {
                                  get_token();
                                  if (source_token[0] == ';')
                                    {
                                      if (evaluate)
                                        condition_is_true
                               =*((*expression_header_ptr).value_ptr.boolean);
                                      else
                                        condition_is_true=FALSE;
                                      do
                                        {
                                          get_token();
                                          if (source_token[0] == ' ')
                                            {
                                              fatal_error=TRUE;
                                              printf(
                                   "Fatal error:  file ends before \"END;\" ");
                                              printf(
                                   "corresponding to \"DO WHILE();\".\n");
                                            }
                                          else
                                            {
                                              if (strcmp(source_token,"END")
                                               != 0)
                                                interpret_statement(
                                                 evaluate && condition_is_true);
                                            }
                                        }
                                      while ((strcmp(source_token,"END") != 0)
                                      &&     (! fatal_error));
                                    }
                                  else
                                    if (source_token[0] == ' ')
                                      {
                                        fatal_error=TRUE;
                                        printf(
                 "Fatal error:  file ends before \";\" of \"DO WHILE();\".\n");
                                      }
                                    else
                                      {
                                        fatal_error=TRUE;
                                        printf(
  "Fatal error:  \";\" of \"DO WHILE();\" expected at line %ld, column %ld.\n",
                                         source_line_num,source_column_num);
                                      }
                                }
                              else
                                if (source_token[0] == ' ')
                                  {
                                    fatal_error=TRUE;
                                    printf(
                   "Fatal error:  file ends before \")\" of \"DO WHILE()\".\n");
                                  }
                                else
                                  {
                                    fatal_error=TRUE;
                                    printf(
   "Fatal error:  \")\" of \"DO WHILE()\" expected at line %ld, column %ld.\n",
                                     source_line_num,source_column_num);
                                  }
                            }
                          else
                            {
                              fatal_error=TRUE;
                              printf(
              "Fatal error:  the expression preceding column %ld on line %d\n",
                               source_column_num,source_line_num);
                              printf(
              "     should be Boolean but isn\'t.\n");
                            }
                          free_value(expression_header_ptr);
                        }
                      if (! fatal_error)
                        {
                          get_token();
                          if (source_token[0] == ' ')
                            {
                              fatal_error=TRUE;
                              printf("Fatal error:  file ends where \";\" ");
                              printf("of \"END;\" expected.\n");
                            }
                          else
                            {
                              if (source_token[0] != ';')
                                {
                                  fatal_error=TRUE;
                                  printf(
                     "Fatal error:  \";\" expected at line %ld, column %ld.\n",
                                   source_line_num,source_column_num);
                                }
                            }
                        }
                    }
                  while((! fatal_error)
                  &&    (evaluate)
                  &&    (condition_is_true));
                }
              else
                {
                  fatal_error=TRUE;
                  printf(
                  "\"(\" of \"DO WHILE(\" expected at line %ld, column %ld.\n",
                   source_line_num,source_column_num);
                }
          }
        else
          if (source_token[0] == ' ')
            {
              fatal_error=TRUE;
              printf(
               "Fatal error:  file ends before \"DO\" statement completed.\n");
            }
          else
            {
              fatal_error=TRUE;
              printf(
        "Fatal error:  \";\" or \"WHILE\" expected at line %ld, column %ld.\n",
               source_line_num,source_column_num);
            }
      return;
    }

static void interpret_if(evaluate)
  int evaluate;
    {
      char             else_char;
      long             else_column_num;
      int              else_eof;
      long             else_line_num;
      int              else_page_index;
      long             else_page_num;
      char             else_token [256];
      value_header_ptr expression_header_ptr;

      expression_header_ptr=interpret_expression(evaluate);
      if (! fatal_error)
        {
          if ((! evaluate)
          ||  ((*expression_header_ptr).type == 'B'))
            {
              get_token();
              if (strcmp(source_token,"THEN") == 0)
                {
                  get_token();
                  if (evaluate)
                    if (*((*expression_header_ptr).value_ptr.boolean))
                      interpret_statement(TRUE);
                    else
                      interpret_statement(FALSE);
                  else
                    interpret_statement(FALSE);
                  else_char=source_char;
                  else_column_num=source_column_num;
                  else_eof=source_eof;
                  else_page_index=page_index;
                  else_page_num=page_num;
                  else_line_num=source_line_num;
                  strcpy(else_token,source_token);
                  get_token();
                  if (strcmp(source_token,"ELSE") == 0)
                    {
                      get_token();
                      if (evaluate)
                        if (*((*expression_header_ptr).value_ptr.boolean))
                          interpret_statement(FALSE);
                        else
                          interpret_statement(TRUE);
                      else
                        interpret_statement(FALSE);
                    }
                  else
                    {
                      if (else_page_num != page_num)
                        {
                          source_index=else_page_num;
                          source_index*=(long) page_size;
                          fseek(source_file,source_index,SEEK_SET);
                          fread(&page[0],1,page_size,source_file);
                        }
                      page_num=else_page_num;
                      page_index=else_page_index;
                      source_char=else_char;
                      source_column_num=else_column_num;
                      source_eof=else_eof;
                      source_line_num=else_line_num;
                      strcpy(source_token,else_token);
                    }
                }
              else
                if (source_token[0] == ' ')
                  {
                    fatal_error=TRUE;
                    printf(
                     "Fatal error:  file ends where \"THEN\" expected.\n");
                  }
                else
                  {
                    fatal_error=TRUE;
                    printf(
                  "Fatal error:  \"THEN\" expected at line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
            }
          else
            {
              fatal_error=TRUE;
              printf(
             "Fatal error:  the expression preceding column %ld on line %ld\n",
               source_column_num,source_line_num);
              printf(
               "     is not a Boolean expression.\n");
            }
          free_value(expression_header_ptr);
        }
      return;
    }

static void interpret_assignment(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      int              comparison;
      value_header_ptr expression_header_ptr;
      int              finished;
      queue_node_ptr   new_queue_copy;
      variable_ptr     new_variable_ptr;
      variable_ptr     old_variable_ptr;
      queue_node_ptr   queue_copy;

      expression_header_ptr=interpret_expression(evaluate);
      if (! fatal_error)
        {
          if (evaluate)
            {
              if (variable_head == NULL)
                if ((variable_head=(struct variable *)
                 malloc((unsigned) sizeof(struct variable))) == NULL)
                  {
                    fatal_error=TRUE;
                    printf("Fatal error:  out of memory at ");
                    printf("line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
                else
                  if (((*variable_head).name
                   =malloc((unsigned) (1+strlen(identifier)))) == NULL)
                    {
                      fatal_error=TRUE;
                      printf("Fatal error:  out of memory at ");
                      printf("line %ld, column %ld.\n",
                       source_line_num,source_column_num);
                      free((char *) variable_head);
                      variable_head=NULL;
                    }
                  else
                    {
                      strcpy((*variable_head).name,identifier);
                      (*variable_head).subscripts=copy_of_queue(queue_head);
                      if (! fatal_error)
                        (*variable_head).variable_value_header_ptr
                         =copy_of_arguments(expression_header_ptr);
                      if (fatal_error)
                        {
                          free((*variable_head).name);
                          free((char *) variable_head);
                          variable_head=NULL;
                        }
                      else
                        {
                          (*variable_head).predecessor_ptr=NULL;
                          (*variable_head).smaller_successor_ptr=NULL;
                          (*variable_head).larger_successor_ptr=NULL;
                        }
                    }
              else
                {
                  old_variable_ptr=variable_head;
                  finished=FALSE;
                  queue_copy=copy_of_queue(queue_head);
                  do
                    {
                      comparison=variable_comparison(identifier,queue_copy,
                       (*old_variable_ptr).name,
                       (*old_variable_ptr).subscripts);
                      if (comparison < 0)
                        if ((*old_variable_ptr).smaller_successor_ptr == NULL)
                          {
                            if ((new_variable_ptr=(struct variable *)
                             malloc((unsigned) sizeof(struct variable)))
                             == NULL)
                              {
                                fatal_error=TRUE;
                                printf("Fatal error:  out of memory at ");
                                printf("line %ld, column %ld.\n",
                                source_line_num,source_column_num);
                              }
                            else
                              if (((*new_variable_ptr).name
                               =malloc((unsigned) (1+strlen(identifier))))
                               == NULL)
                                {
                                  fatal_error=TRUE;
                                  printf("Fatal error:  out of memory at ");
                                  printf("line %ld, column %ld.\n",
                                   source_line_num,source_column_num);
                                  free((char *) variable_head);
                                  variable_head=NULL;
                                }
                              else
                                {
                                  strcpy((*new_variable_ptr).name,identifier);
                                  (*new_variable_ptr).subscripts=queue_copy;
                                  if (! fatal_error)
                                    (*new_variable_ptr).
                                     variable_value_header_ptr
                                     =copy_of_arguments(expression_header_ptr);
                                  if (fatal_error)
                                    {
                                      free((*new_variable_ptr).name);
                                      free((char *) new_variable_ptr);
                                      new_variable_ptr=NULL;
                                    }
                                  else
                                    {
                                      (*new_variable_ptr).predecessor_ptr
                                       =old_variable_ptr;
                                      (*new_variable_ptr).
                                       smaller_successor_ptr=NULL;
                                      (*new_variable_ptr).
                                       larger_successor_ptr=NULL;
                                      (*old_variable_ptr).
                                       smaller_successor_ptr
                                       =new_variable_ptr;
                                    }
                                }
                            finished=TRUE;
                          }
                        else
                          old_variable_ptr
                           =(*old_variable_ptr).smaller_successor_ptr;
                      else
                        if (comparison > 0)
                          if ((*old_variable_ptr).larger_successor_ptr
                           == NULL)
                            {
                              if ((new_variable_ptr=(struct variable *)
                               malloc((unsigned) sizeof(struct variable)))
                               == NULL)
                                {
                                  fatal_error=TRUE;
                                  printf("Fatal error:  out of memory at ");
                                  printf("line %ld, column %ld.\n",
                                  source_line_num,source_column_num);
                                }
                              else
                                if (((*new_variable_ptr).name
                                 =malloc((unsigned) (1+strlen(identifier))))
                                 == NULL)
                                  {
                                    fatal_error=TRUE;
                                    printf("Fatal error:  out of memory at ");
                                    printf("line %ld, column %ld.\n",
                                     source_line_num,source_column_num);
                                    free((char *) variable_head);
                                    variable_head=NULL;
                                  }
                                else
                                  {
                                    strcpy((*new_variable_ptr).name,
                                     identifier);
                                    (*new_variable_ptr).subscripts=queue_copy;
                                    if (! fatal_error)
                                      (*new_variable_ptr).
                                       variable_value_header_ptr
                                       =copy_of_arguments(
                                       expression_header_ptr);
                                    if (fatal_error)
                                      {
                                        free((*new_variable_ptr).name);
                                        free((char *) new_variable_ptr);
                                        new_variable_ptr=NULL;
                                      }
                                    else
                                      {
                                        (*new_variable_ptr).predecessor_ptr
                                         =old_variable_ptr;
                                        (*new_variable_ptr).
                                         smaller_successor_ptr=NULL;
                                        (*new_variable_ptr).
                                         larger_successor_ptr=NULL;
                                        (*old_variable_ptr).
                                         larger_successor_ptr
                                         =new_variable_ptr;
                                      }
                                  }
                              finished=TRUE;
                            }
                          else
                            old_variable_ptr
                             =(*old_variable_ptr).larger_successor_ptr;
                        else
                          {
                            finished=TRUE;
                            while (queue_copy != NULL)
                              {
                                new_queue_copy=(*queue_copy).next;
                                free_value((*queue_copy).argument_header_ptr);
                                free((char *) queue_copy);
                                queue_copy=new_queue_copy;
                              }
                            free_value(
                             (*old_variable_ptr).variable_value_header_ptr);
                            (*old_variable_ptr).variable_value_header_ptr
                             =copy_of_arguments(expression_header_ptr);
                          }
                    }
                  while (! finished);
                }
              free_value(expression_header_ptr);
            }
        }
      if (! fatal_error)
        {
          get_token();
          if (source_token[0] == ' ')
            {
              fatal_error=TRUE;
              printf(
               "Fatal error:  file ends where \";\" expected.\n");
            }
          else
            {
              if (source_token[0] != ';')
                {
                  fatal_error=TRUE;
                  printf(
                   "Fatal error:  \";\" expected at line %ld, column %ld.\n",
                   source_line_num,source_column_num);
                }
            }
        }
    }

static void perform_close(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      if (queue_head == NULL)
        {
          if (evaluate)
            fclose(stdin);
        }
      else
        if ((*queue_head).next == NULL)
          {
            if (evaluate)
              {
                if ((*((*queue_head).argument_header_ptr)).type == 'D')
                  fclose(
                   *((*((*queue_head).argument_header_ptr)).value_ptr.dataset));
                else
                  {
                    fatal_error=TRUE;
                    printf(
                 "Fatal error:  argument to CLOSE is not a file pointer on\n");
                    printf("     line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
              }
          }
        else
          {
            fatal_error=TRUE;
            printf(
             "Fatal error:  extraneous arguments supplied to CLOSE on\n");
            printf("     line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
      return;
    }

static void perform_clrscr(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
#ifdef INCL_BASE
      unsigned char fill [2];
      VIOMODEINFO   video_mode_info;
#else
      union REGS inreg;
      union REGS outreg;
#endif

      if (queue_head == NULL)
        {
          if (evaluate)
            {
#ifdef INCL_BASE
              video_mode_info.cb=(unsigned int) 12;
              VioGetMode(&video_mode_info,(HVIO) 0);
              fill[0]=(unsigned char) ' ';
              fill[1]=(unsigned char) 7;
              VioScrollUp((USHORT) 0,(USHORT) 0,
               (USHORT) (video_mode_info.row-1),
               (USHORT) (video_mode_info.col-1),
               (USHORT) 0xffff,(PBYTE) &fill[0],(HVIO) 0);
              VioSetCurPos((USHORT) 0,(USHORT) 0,(HVIO) 0);
 #else
              inreg.h.ah=(unsigned char) 15;
              int86(16,&inreg,&outreg);
              inreg.h.ah=(unsigned char) 0;
              inreg.h.al=outreg.h.al;
              int86(16,&inreg,&outreg);
 #endif
            }
        }
      else
        {
          fatal_error=TRUE;
          printf(
           "Fatal error:  extraneous arguments supplied to CLOSE on\n");
          printf("     line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      return;
    }

static void perform_pliretc(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          printf(
           "Fatal error:  argument not supplied to PLIRETC on\n");
          printf("     line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      else
        if ((*queue_head).next == NULL)
          {
            if (evaluate)
              {
                if ((*((*queue_head).argument_header_ptr)).type == 'I')
                  if ((*((*((*queue_head).argument_header_ptr)).value_ptr.
                   integer) > (long) 999)
                  ||  (*((*((*queue_head).argument_header_ptr)).value_ptr.
                   integer) < (long) 0))
                    return_code=999;
                  else
                    return_code=(int)
                     *((*((*queue_head).argument_header_ptr)).value_ptr.
                     integer);
                else
                  {
                    fatal_error=TRUE;
                    printf(
                   "Fatal error:  argument to PLIRETC is not an integer on\n");
                    printf("     line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
              }
          }
        else
          {
            fatal_error=TRUE;
            printf(
             "Fatal error:  extraneous arguments supplied to PLIRETC on\n");
            printf("     line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
      return;
    }

static void perform_print(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      register int  char_index;
      unsigned char *char_ptr;
      FILE          *file;
      int           string_length;

      if (queue_head == NULL)
        {
          fatal_error=TRUE;
          printf(
           "Fatal error:  first parameter to PRINT is missing on\n");
          printf(
           "     line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      else
        {
          if (evaluate)
            {
              if ((*((*queue_head).argument_header_ptr)).type == 'D')
                {
                  file=
                   *((*((*queue_head).argument_header_ptr)).value_ptr.dataset);
                  queue_head=(*queue_head).next;
                  while (queue_head != NULL)
                    {
                      switch ((*((*queue_head).argument_header_ptr)).type)
                        {
                          case 'B':
                            if (*((*((*queue_head).argument_header_ptr)).
                             value_ptr.boolean))
                              fprintf(file,"TRUE");
                            else
                              fprintf(file,"FALSE");
                            break;
                          case 'D':
                            fprintf(file,"%p",(char far *)
                             *((*((*queue_head).argument_header_ptr)).
                             value_ptr.dataset));
                            break;
                          case 'I':
                            fprintf(file,"%ld",
                             *((*((*queue_head).argument_header_ptr)).
                             value_ptr.integer));
                            break;
                          case 'R':
                            fprintf(file,"%lG",
                             *((*((*queue_head).argument_header_ptr)).
                             value_ptr.real));
                            break;
                          default:
                            char_ptr=(*((*((*queue_head).argument_header_ptr)).
                             value_ptr.string)).value;
                            string_length
                             =(*((*((*queue_head).argument_header_ptr)).
                             value_ptr.string)).length;
                            for (char_index=0; char_index < string_length;
                             char_index++)
                              {
                                fputc((int) *char_ptr,file);
                                char_ptr++;
                              }
                            break;
                        }
                      queue_head=(*queue_head).next;
                    }
                }
              else
                {
                  fatal_error=TRUE;
                  printf(
          "Fatal error:  first parameter to PRINT is not a file pointer on\n");
                  printf(
                   "     line %ld, column %ld.\n",source_line_num,
                   source_column_num);
                }
            }
        }
      return;
    }

static void perform_putcrlf(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      if (queue_head == NULL)
        {
          if (evaluate)
            printf("\n");
        }
      else
        if ((*queue_head).next == NULL)
          {
            if (evaluate)
              {
                if ((*((*queue_head).argument_header_ptr)).type == 'D')
                  fprintf(
                   *((*((*queue_head).argument_header_ptr)).value_ptr.dataset),
                   "\n");
                else
                  {
                    fatal_error=TRUE;
                    printf(
               "Fatal error:  argument to PUTCRLF is not a file pointer on\n");
                    printf("     line %ld, column %ld.\n",
                     source_line_num,source_column_num);
                  }
              }
          }
        else
          {
            fatal_error=TRUE;
            printf(
             "Fatal error:  extraneous arguments supplied to PUTCRLF on\n");
            printf("     line %ld, column %ld.\n",
             source_line_num,source_column_num);
          }
      return;
    }

static void perform_troff(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      if (queue_head == NULL)
        {
          if (evaluate)
            trace=FALSE;
        }
      else
        {
          fatal_error=TRUE;
          printf(
           "Fatal error:  extraneous arguments supplied to TROFF on\n");
          printf("     line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      return;
    }

static void perform_tron(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      if (queue_head == NULL)
        {
          if (evaluate)
            trace=TRUE;
        }
      else
        {
          fatal_error=TRUE;
          printf(
           "Fatal error:  extraneous arguments supplied to TRON on\n");
          printf("     line %ld, column %ld.\n",
           source_line_num,source_column_num);
        }
      return;
    }

static void interpret_procedure(evaluate,queue_head)
  int            evaluate;
  queue_node_ptr queue_head;
    {
      if      (strcmp(identifier,"CLOSE") == 0)
        perform_close(evaluate,queue_head);
      else if (strcmp(identifier,"CLRSCR") == 0)
        perform_clrscr(evaluate,queue_head);
      else if (strcmp(identifier,"PLIRETC") == 0)
        perform_pliretc(evaluate,queue_head);
      else if (strcmp(identifier,"PRINT") == 0)
        perform_print(evaluate,queue_head);
      else if (strcmp(identifier,"PUTCRLF") == 0)
        perform_putcrlf(evaluate,queue_head);
      else if (strcmp(identifier,"TROFF") == 0)
        perform_troff(evaluate,queue_head);
      else if (strcmp(identifier,"TRON") == 0)
        perform_tron(evaluate,queue_head);
      else
        {
          fatal_error=TRUE;
          printf(
           "Fatal error:  unrecognized procedure \"%s\" at ",
           identifier);
          printf(
           "line %ld, column %ld.\n",source_line_num,
           source_column_num);
        }
      return;
    }

static void interpret_statement(evaluate)
  int evaluate;
    {
      queue_node_ptr new_queue_head;
      queue_node_ptr new_queue_tail;
      queue_node_ptr queue_head;
      queue_node_ptr queue_tail;

      if ((evaluate) && (trace))
        printf("[%ld]",source_line_num);
      if (source_token[0] == ' ')
        {
          fatal_error=TRUE;
          printf("Fatal error:  end of file encountered where statement ");
          printf("expected.\n");
        }
      else
        if (strcmp(source_token,"DO") == 0)
          interpret_do(evaluate);
        else
          if (strcmp(source_token,"IF") == 0)
            interpret_if(evaluate);
          else
            {
              if (strcmp(source_token,";") != 0)
                {
                  if (isalpha((int) source_token[0]))
                    {
                      queue_tail=NULL;
                      queue_head=NULL;
                      strcpy(identifier,source_token);
                      get_token();
                      if (source_token[0] == '(')
                        {
                          if ((queue_head=(queue_node_ptr)
                           malloc((unsigned) sizeof(struct queue_node)))
                           == NULL)
                            {
                              fatal_error=TRUE;
                              printf(
                      "Fatal error:  out of memory at line %ld, column %ld.\n",
                               source_line_num,source_column_num);
                            }
                          else
                            {
                              queue_tail=queue_head;
                              (*queue_head).next=NULL;
                              (*queue_head).argument_header_ptr
                               =interpret_expression(evaluate);
                            }
                          if (! fatal_error)
                            get_token();
                          while ((! fatal_error)
                          &&     (! source_eof)
                          &&     (source_token[0] != ')'))
                            {
                              if ((new_queue_tail=(queue_node_ptr)
                               malloc((unsigned) sizeof(struct queue_node)))
                               == NULL)
                                {
                                  fatal_error=TRUE;
                                  printf(
                      "Fatal error:  out of memory at line %ld, column %ld.\n",
                                   source_line_num,source_column_num);
                                }
                              else
                                {
                                  (*new_queue_tail).next=NULL;
                                  (*queue_tail).next=new_queue_tail;
                                  queue_tail=new_queue_tail;
                                  (*new_queue_tail).argument_header_ptr
                                   =interpret_expression(evaluate);
                                }
                              if (! fatal_error)
                                get_token();
                            }
                          if (! fatal_error)
                            {
                              if (source_token [0] == ')')
                                get_token();
                            }
                        }
                      if (! fatal_error)
                        {
                          if (source_token[0] == '=')
                            interpret_assignment(evaluate,queue_head);
                          else
                            if (source_token[0] == ';')
                              interpret_procedure(evaluate,queue_head);
                            else
                              if (source_token[0] == ' ')
                                {
                                  fatal_error=TRUE;
                                  printf(
                   "Fatal error:  file ends where \"=\" or \";\" expected.\n");
                                }
                              else
                                {
                                  fatal_error=TRUE;
                                  printf(
                       "Fatal error:  \"=\", or \";\" expected at line %ld,\n",
                                   source_line_num);
                                  printf(
                                   "     column %ld.\n",source_column_num);
                                }
                        }
                      while (queue_head != NULL)
                        {
                          new_queue_head=(*queue_head).next;
                          free_value((*queue_head).argument_header_ptr);
                          free((char *) queue_head);
                          queue_head=new_queue_head;
                        }
                    }
                  else
                    if (source_token[0] == ' ')
                      {
                        fatal_error=TRUE;
                        printf(
          "Fatal error:  end of file encountered where statement expected.\n");
                      }
                    else
                      {
                        fatal_error=TRUE;
                        printf("Fatal error:  expected statement at ");
                        printf("     line %ld, column %ld.\n",source_line_num,
                         source_column_num);
                      }
                }
            }
      return;
    }

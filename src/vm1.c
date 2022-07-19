#include <s1.h>
#include <lib.c>
#include <ir.c>
#include <env.c>

void parse_ir(char *line, char *op, char *arg) {
  *op='\0'; *arg='\0';
  sscanf(line, "%s %s", op, arg);
  if (strcmp(op, "str")==0) {
    sscanf(line, "%s '%[^']", op, arg);
  }
}

int asm2ir(char *line) {
  char p1[101], p2[101];
  debug("%s", line);
  parse_ir(line, p1, p2);
  ir_t op = op_code(p1);
  // debug("op=%d:\n", (int) op);
  if (op < 0 || op==Src) return -1;
  eir(op);
  char *s=NULL; int n; double f;
  if (op == Float) {
    sscanf(p2, "%lf", &f);
    debug("=> %s %lf\n", p1, f);
    eir(f);
  } else if (op == Str || op == Get || op == Var || op == Fn) {
    s = st_printf("%s", p2);
    debug("=> %s %s\n", p1, s);
    eir(s);
  } else if (op == Narg || op == Ent || op == Jmp || op == Bz || op == Bnz || op == Adj) {
    n = atoi(p2);
    debug("=> %s %d\n", p1, n);
    eir(n);
  } else {
    debug("=> %s\n", p1);
  }
  return 0;
}

void watch(char *head, struct obj **sp, struct obj *a) {
  if (!dbg) return; 
  debug(" %s a=", head); o_print(a); 
  if (sp > ostack) {
    debug(" top="); o_print(*(sp-1));
  }
  debug("\n");
}

// #define code_obj(pc) code_o[(pc)-code]
int run() {
  printf("==================run================\n");
  env_init();
  ir_t *pc = code;
  struct obj *a;
  bool is_stop = false;

  while (1) {
    if (pc >= cp) error("no more code...\n");
    ir_t op = *pc++;
    char opname[20];
    op_name(op, opname);
    debug("%s ", opname);
    char *s; int n; double f; struct var *v; struct obj *o;

    if (op == Float) {
      f = (double) *pc++;
      debug(" %lf", f);
    } else if (op == Str) {
      s = (char*) *pc++;
      debug(" '%s'", s);
    } else if (op == Get || op == Var || op == Fn || op == Src) {
      s = (char*) *pc++;
      debug(" %s", s);
    } else if (op == Local || op == Narg || op == Ent || op == Jmp || op == Bz || op == Bnz || op == Adj) {
      n = *pc++;
      debug(" %d", n);
    }
    debug("\n");

    watch("before:", sp, a);
    switch (op) {
      case Var:
        v = env_push_var(s);
        a = v->o = env_new_obj(TNONE);
        break;
      case Push: case Arg:
        *sp++ = a;
        break;
      case Get:
        v=env_get_var(s);
        if (!v) error("Error: get %s fail!\n", s);
        a = v->o;
        break;
      case Store:
        o = *--sp;
        *o = *a;
        break;
      case Str:
        a = env_new_obj(TSTRING);
        a->str = s;
        break;
      case Float:
        a = env_new_obj(TFLOAT);
        a->f = f;
        break;
      case '!':
        a = o_lnot(a);
        break;
      case Neg:
        a = o_neg(a);
        break;
      case '+':
        a = o_add(*--sp, a);
        break;
      case '-':
        a = o_sub(*--sp, a);
        break;
      case '*':
        a = o_mul(*--sp, a);
        break;
      case '/':
        a = o_div(*--sp, a);
        break;
      case '%':
        a = o_mod(*--sp, a);
        break;
      case Eq:
        a = o_eq(*--sp, a);
        break;
      case Neq:
        a = o_neq(*--sp, a);
        break;
      case Le:
        a = o_le(*--sp, a);
        break;
      case Ge:
        a = o_ge(*--sp, a);
        break;
      case '<':
        a = o_lt(*--sp, a);
        break;
      case '>':
        a = o_gt(*--sp, a);
        break;
      case Shl:
        a = o_shl(*--sp, a);
        break;
      case Shr:
        a = o_shr(*--sp, a);
        break;
      case '&':
        a = o_band(*--sp, a);
        break;
      case '|':
        a = o_bor(*--sp, a);
        break;
      case '^':
        a = o_bxor(*--sp, a);
        break;
      case Lor:
        a = o_lor(*--sp, a);
        break;
      case Land:
        a = o_land(*--sp, a);
        break;
      case Call:
        o = *--sp;
        a = o_call(o, a);
        break;
      case Narg:
        a = env_new_array(n);
        for (int i=n-1; i>=0; i--) {
          a->a[i] = *--sp;
        }
        break;
/*
      case Fn:
        env_pushf(s); // 新增函數堆疊，將參數加入其中
        break;
      case Param: 
        v = env_push_var(s);
        // v->o = env_paramobj(s); // 綁定 param 到 frame 的物件
        break;
      case Ent:
        env_params_end();
        break;
      case Lev: case Ret: 
        env_popf();
        break;
      case Jmp:
        pc = pc + n;
        break;
      case Bz:
        if (o_iszero(a)) pc = pc + n;
        break;
      case Bnz:
        if (!o_iszero(a)) pc = pc + n;
        break;

      case Local:
        a = env_get_local(n)->o;
        break;
*/
      default:
        error("op=%d not handled!", (int) op);
    }
    watch("after :", sp, a);
    if (is_stop) break;
  }
  return 0;
}

int main(int argc, char **argv) {
  // dbg = 1;
  if (argc < 2) error("%s <ir_file>\n", argv[0]);
  char *ifile = argv[1];

  FILE *f = fopen(ifile, "r");
  if (!f) error("open %s fail!\n", ifile);
  while (!feof(f)) {
    char line[256];
    fgets(line, sizeof(line), f);
    asm2ir(line);
  }
  fclose(f);

  run();
}

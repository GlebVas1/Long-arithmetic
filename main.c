#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#define BASE 46340 //Система счисления для числа, решил вынести, чтобы сразу можно было менять во всем коде
typedef int typeofmem; //Тип памяти

struct bn_s
{
    typeofmem* body;
    char s;
    int bodysize;
};
typedef struct bn_s bn;
bn *bn_new(); //+
bn *bn_init(bn const *orig); //+
int bn_init_string(bn *t, const char *init_string); //+
int bn_copy(bn*t, bn const *orig);
int bn_init_string_radix(bn *t, const char *init_string, int radix);//+
int bn_init_int(bn *t, int init_int); //+
int bn_delete(bn *t); //+
int bn_add_to(bn *t, bn const *right); //+
int bn_sub_to(bn *t, bn const *right); //+
int bn_mul_to(bn *t, bn const *right); //+
int bn_div_to(bn *t, bn const *right); //+
int bn_mod_to(bn *t, bn const *right); //+
int bn_pow_to(bn *t, int degree); //+
int bn_root_to(bn *t, int reciprocal);
bn* bn_add(bn const *left, bn const *right); //+
bn* bn_sub(bn const *left, bn const *right); //+
bn* bn_mul(bn const *left, bn const *right); //+
bn* bn_div(bn const *left, bn const *right); //+
bn* bn_mod(bn const *left, bn const *right); //+
const char *bn_to_string(bn const *t, int radix);
int bn_cmp(bn const *left, bn const *right); //+
int bn_neg(bn *t); //+
int bn_abs(bn *t); //+
int bn_sign(bn const *t); //+
int bn_check(bn const *t) //защита от дурака или системного сбоя
{
    if(t->body==NULL)
    {
        return 1;
    }
    if(t->s<-1||t->s>1)
    {
        return 1;
    }
    if(t->bodysize<0)
    {
        return 1;
    }
    return 0;
}
char int_to_ch(int i) //для преобразовния в строку
{
    if(i>=0 && i <= 9)
        return '0'+i;
    if(i>=10)
        return 'A'+i-10;
    return '*';
}
int bn_copy(bn*t, bn const *orig)
{
    if(t==NULL|| bn_check(orig) == 1)
    {
        return 1;
    }

    free(t->body);
    t->body = (typeofmem*)malloc(orig->bodysize*sizeof(typeofmem));
    t->s=orig->s;
    t->bodysize=orig->bodysize;
    for(int i = 0; i < t->bodysize; i++)
    {
        t->body[i]=orig->body[i];
    }
    return 0;
}
int bn_allocmemory(bn *t, int size) //функция выделения памяти в теле числа, проверку не имеет, т.к. не вызывается пользователем, а только в библиотечной функции
{
    if(t==NULL)
        return 1;
    free(t->body);
    t->bodysize = size;
    t->body = (typeofmem*)malloc(t->bodysize*sizeof(typeofmem));
    return 0;
}
int bn_show(bn const *t) //для дебага в дестичной системе, чтобы смотреть как примерно работает функция
{
    if(t->s==-1)
        putchar('-');
    for(int i = 0; i < t->bodysize; i++)
    {
        putchar(int_to_ch(t->body[t->bodysize-1-i]));
    }
    printf("\n");
    return 0;
}
enum bn_codes {
BN_OK, BN_NULL_OBJECT, BN_NO_MEMORY, BN_DIVIDE_BY_ZERO
};
bn *bn_new()
{
    bn *r = (bn*)malloc(sizeof(bn));
    r->body = (typeofmem*)malloc(1*sizeof(typeofmem));
    r->bodysize=1;
    r->body[0]=0;
    r->s=0; //Ставлю в ноль
    return r;
};
bn *bn_init(bn const *orig)
{
    if(bn_check(orig)==1)
        return NULL;
    bn *r = bn_new();
    bn_allocmemory(r,orig->bodysize);
    r->s=orig->s;
    for(int i = 0; i <orig->bodysize; i++)
    {
        r->body[i]=orig->body[i];
    }
    return r;
}// Со(дат+ копи1 существу1щего BN
int bn_init_string(bn *t, const char *init_string)
{
    if(bn_check(t)==1)
            return 1;
    bn_init_string_radix(t,init_string,10);
    return 0;
}
// Инициали(ироват+ (начение BN представлением строки
// в системе счислени: radix
int bn_init_string_radix(bn *t, const char *init_string, int radix)
{
    if(bn_check(t)== 1)
        return 1;
    int length=0;
    bn_init_int(t,0);
    while(init_string[length]!='\0' && init_string[length]!='\n' )
    {
        length++;
    }
    if(length ==0)
    {
         bn_init_int(t,0);
         return 0;
    }
    int perev=0;
     if(init_string[0]=='-')
     {
         perev=1;
     }

    int rad = 0;
     bn *a = bn_new();
     bn *b = bn_new();
     for(int i=0; i<=length-1-perev; i++)
     {
        rad=init_string[length-1-i];
        if(rad<='9')
            rad -='0';
        if(rad>='a'&&rad<='z')
            rad -='a'-10;
        if(rad>='A'&&rad<='Z')
            rad -='A'-10;
        bn_init_int(a,rad);
        bn_init_int(b,radix);
        bn_pow_to(b,i);
        bn_mul_to(b,a);
        bn_add_to(t,b);
     }
    if(perev==1)
        t->s=-1;
    if(t->bodysize==1 && t->body[0]==0)
        t->s=0;
     bn_delete(a);
     bn_delete(b);

     return 0;
}
int bn_init_int(bn *t, int init_int)
{
    if(t == NULL)
        return 1;
    if(init_int == 0)
    {
        t->s=0;
        t->bodysize=1;
        free(t->body);
        t->body =(typeofmem*)malloc(1*sizeof(typeofmem));
        t->body[0]=0;
        return 0;
    }
    if(init_int < 0)
    {
        init_int*=-1;
        t->s=-1;
    }
    else
        t->s=1;
    int countint = init_int;
    int c=0;
    while(countint>0)
    {
        countint/=BASE;
        c++;
    }
    bn_allocmemory(t,c);
    for(int i = 0; i < c; i++)
    {
        t->body[i]=init_int%BASE;
        init_int/=BASE;
    }
    return 0;
}
int bn_delete(bn *t)
{
    if(t!=NULL)
    {
        if(t->body!=NULL)
        free(t->body);
        free(t);
        return 0;
    }
    return 1;
}
int bn_add_to(bn *t, bn const *right)
{
    if(bn_check(t)==1 || bn_check(right)==1)
        return 1;
    bn *r;
    r = bn_add(t,right);
    bn_copy(t,r);
    bn_delete(r);
    return 0;
}
int bn_sub_to(bn *t, bn const *right)
{
    if(bn_check(t)==1 || bn_check(right)==1)
        return 1;
    bn *r;
    r = bn_sub(t,right);
    bn_copy(t,r);
    bn_delete(r);
    return 0;
}
int bn_mul_to(bn *t, bn const *right)
{
    if(bn_check(t)==1 || bn_check(right)==1)
        return 1;
    bn *r;
    r = bn_mul(t,right);
    bn_copy(t,r);
    bn_delete(r);
    return 0;
}
int bn_div_to(bn *t, bn const *right)
{
    if(bn_check(t)==1 || bn_check(right)==1)
        return 1;
    bn *r;
    r = bn_div(t,right);
    if (r==NULL)
    {
        bn_delete(r);
        return 4; //деление возвращает NULL если делится на 0
    }
    bn_copy(t,r);
    bn_delete(r);
    return 0;
}
int bn_mod_to(bn *t, bn const *right)
{
    if(bn_check(t)==1 || bn_check(right)==1)
        return 1;
    bn *r;
    r = bn_mod(t,right);
    if (r==NULL)
    {
        bn_delete(r);
        return 4; //остаток возвращает NULL если делится на 0
    }
    bn_copy(t,r);
    bn_delete(r);
    return 0;
}
// Во(вести число в степен+ degree
bn* my_pow(bn* t, int deg)
{
     if(bn_check(t)==1)
        return NULL;
     //Взял из книги
     bn *r = NULL;
     if(deg == 0)
     {
         r=bn_new();
         bn_init_int(r,1); //тут спорный момент, если число = 0 и deg = 0. То можно было бы вывести и ошибку, но, как оказалось некоторые калькуляторы вполне выдают ответ, либо 0 либо 1
         return r;
     }
     if(deg % 2 != 0)
     {
        bn_delete(r);
        bn *a = my_pow(t, deg-1);
        bn *b = bn_mul(t,a);
        bn_delete(a);

        return b;
     }
     bn *a = my_pow(t,deg/2);
     r = bn_mul(a,a);
     bn_delete(a);
     return r;
}
int bn_pow_to(bn *t, int degree)
{
    if(bn_check(t)==1)
        return 1;
    bn *r = my_pow(t, degree);
    bn_copy(t,r);
    bn_delete(r);
    return 0;
}

int bn_root_to(bn *t, int reciprocal)
{
    if(bn_check(t)==1)
        return 1;
    if(t->s<=0)
    return 4; //отдельного enum'a под ошибку не было, решил не усложнять, а использовать существующий связанный с нулем
    if(reciprocal==1) //для чисел больших 1 функция не выполняется
    return 0;
    if(reciprocal==0)
    return 4; //степень вида 1/n n=0
    bn *dva = bn_new();
    bn_init_int(dva,2);
    if(bn_cmp(t,dva)<=0)
    {
        bn *od = bn_new();
        bn_init_int(od,1);
        bn_copy(t,od);
        bn_delete(od);
        bn_delete(dva);
        return 0;
    }

    if(reciprocal>2)
    {
        bn *n = bn_new();
        bn_init_int(n,reciprocal);
        bn *n_1 = bn_new();
        bn_init_int(n_1,reciprocal-1);
        bn *x=bn_new();
        bn_allocmemory(x,t->bodysize/2+1);
         //Заметил, что в н-ричной записи, что целый корень от целого положительного числа
         //будет более чем в половину меньше (по длине записи), чем само число. В принципе это легко заметить,
         //если в числе страший разряд BASE^n то в корне BASE^n/2. для верности добавил ещё один. Думаю, что
         //можно на самом деле было поделить и на n.
        memset(x->body,0,sizeof(typeofmem)*x->bodysize);
        x->body[x->bodysize-1]=BASE-1;
        x->s=1;
        bn*oldx = bn_init(x);
        do
        {
            bn_copy(oldx,x);
            bn* a = bn_init(x);
            bn_pow_to(a,reciprocal-1);
            bn* b = bn_div(t,a);
            bn_delete(a);
            a = bn_mul(x,n_1);
            bn_add_to(b,a);
            bn_div_to(b,n);
            bn_copy(x,b);
            bn_delete(a);
            bn_delete(b);
        }while(bn_cmp(x,oldx)<0);
        bn_delete(n);
        bn_delete(n_1);
        bn_copy(t,oldx);
        bn_delete(x);
        bn_delete(oldx);
    }
    else
    {
        bn *x=bn_new();
        bn_allocmemory(x,t->bodysize/2+1);
        //Заметил, что в н-ричной записи, что целый корень от целого положительного числа
        //будет более чем в половину меньше (по длине записи), чем само число. В принципе это легко заметить,
        //если в числе страший разряд BASE^n то в корне BASE^n/2. для верности добавил ещё один.
        memset(x->body,0,sizeof(typeofmem)*x->bodysize);
        x->body[x->bodysize-1]=BASE-1;
        x->s=1;
        bn*oldx = bn_init(x);
        do
        {
            bn_copy(oldx,x);
            bn* a = bn_div(t,x);
            bn_add_to(a,x);
            bn_div_to(a,dva);
            bn_copy(x,a);
            bn_delete(a);
        }while(bn_cmp(x,oldx)<0);
        bn_copy(t,oldx);
        bn_delete(x);
        bn_delete(oldx);
    }
    bn_delete(dva);
    return 0;
}


bn* bn_add(bn const *left, bn const *right)
{
    if(bn_check(left)==1 || bn_check(right)==1)
        return NULL;
    bn *r = NULL;
    if(left->s==0)
    {
        r= bn_init(right);
        return r;
    }
    if(right->s==0)
    {
        r = bn_init(left);
        return r;
    }
    if(left->s == -1 && right-> s==-1)
    {
        bn *nl = bn_init(left);
        bn *nr = bn_init(right);
        bn_abs(nl);
        bn_abs(nr);
        r = bn_add(nr,nl);
        r->s=-1;
        bn_delete(nl);
        bn_delete(nr);
        return r;
    }
    if(left->s==1 && right->s==-1)
    {
         bn *nr = bn_init(right);
         bn_abs(nr);
         r = bn_sub(left,nr);
         bn_delete(nr);
         return r;
    }
    if(left->s==-1 && right->s==1)
    {
         bn *nl = bn_init(left);
         bn_abs(nl);
         r = bn_sub(right,nl);
         bn_delete(nl);
         return r;
    }
    if(left->s == 1 && right-> s==1)
    {
        if(bn_cmp(left,right)>=0)
        {
             bn *r = bn_init(left);
             int p1 = 0;
             for(int i = 0; i < right->bodysize; i++)
             {
                r->body[i]=r->body[i]+right->body[i]+p1;
                if(p1>0)
                {
                    p1=0;
                }
                if(r->body[i]>=BASE)
                {
                  r->body[i]-=BASE;
                  p1=1;
                }
             }
             for(int i = right->bodysize; i < left->bodysize; i++)
             {
                r->body[i]=left->body[i]+p1;
                if(p1>0)
                {
                    p1=0;
                }
                if(r->body[i]>=BASE)
                {
                  r->body[i]%=BASE;
                  p1=1;
                }
             }
             if(p1==1)
             {
                 bn *r1 = bn_new();
                 bn_allocmemory(r1,r->bodysize+1);
                 r1->s=1;
                 for(int i = 0; i < r->bodysize; i++)
                 {
                     r1->body[i]=r->body[i];
                 }
                 r1->body[r->bodysize]=1;
                 bn_delete(r);
                 return r1;
             }
             return r;
        }
        else
        {
            return bn_add(right,left);
        }
    }
    return r;
}
bn* bn_sub(bn const *left, bn const *right)
{
    if(bn_check(left)==1 || bn_check(right)==1)
        return NULL;
    bn *r = NULL;
     if(left->s==0&&right->s==0)
    {
        bn *r = bn_new();
        return r;
    }
    if(left->s==0)
    {
        r= bn_init(right);
        r->s=-1;
        return r;
    }
    if(right->s==0)
    {
        r = bn_init(left);
        return r;
    }
    if(left->s == 1 && right-> s==1)
    {
        if(bn_cmp(left,right)>0)
        {
                r = bn_new();
                bn_init_int(r,1);
                bn_allocmemory(r,left->bodysize);
                int p1=0;
                for(int i = 0; i < right->bodysize; i++)
                {
                    r->body[i]=left->body[i]-right->body[i]+p1;
                    if(p1<0) p1=0;
                    if(r->body[i]<0)
                    {
                        r->body[i]+=BASE;
                        p1=-1;
                    }
                }
                for(int i = right->bodysize; i < left->bodysize; i++)
                {
                    r->body[i]=left->body[i]+p1;
                    if(p1<0) p1=0;
                    if(r->body[i]<0)
                    {
                        r->body[i]+=BASE;
                        p1=-1;
                    }
                }

                int newbodsize=0;
                for(int i = r->bodysize-1; i >=0; i--)
                {
                    if(r->body[i]!=0)
                    {
                       newbodsize=i+1;
                        break;
                    }
                }
                bn *r1 = bn_new();
                bn_allocmemory(r1,newbodsize);
                r1->s=1;
                for(int i =0; i < newbodsize; i++)
                {
                    r1->body[i]=r->body[i];
                }
                bn_delete(r);
                return r1;
        }
        if(bn_cmp(left,right)<0)
        {
            r=bn_sub(right,left);
            r->s=-1;
            return r;
        }
        if(bn_cmp(left,right)==0)
        {
            r= bn_new();
            return r;
        }
        return r;
    }
    if(left->s == 1 && right-> s==-1)
    {
        bn *q = bn_init(right);
        q->s=1;
        r= bn_add(left,q);
        bn_delete(q);
        return r;
    }
    if(left->s == -1 && right-> s==1)
    {

        bn *q = bn_init(right);
        q->s=-1;
        r= bn_add(left,q);
        bn_delete(q);
        return r;
    }
    if(left->s == -1 && right-> s==-1)
    {
        bn *nl = bn_init(left);
        bn *nr = bn_init(right);
        bn_abs(nl);
        bn_abs(nr);
        r = bn_sub(nl,nr);
        bn_neg(r);
        if(r->bodysize==1 && r->body[0]==0)
        {
            r->s=0;
        }
        bn_delete(nl);
        bn_delete(nr);
        return r;
    }
    return r;
}
bn* bn_mul(bn const *left, bn const *right)
{
    if(bn_check(left)==1 || bn_check(right)==1)
        return NULL;
    if(right->s==0 || left->s==0)
    {
        bn *r = bn_new();
        bn_init_int(r,0);
        return r;
    }
    bn *res = bn_new();
    bn *nr = bn_init(right);
    bn *nl = bn_init(left);
    bn_abs(nr);
    bn_abs(nl);
    bn_allocmemory(res,nl->bodysize+nr->bodysize);
        int p1=0;
        memset(res->body,0,sizeof(typeofmem)*(nl->bodysize+nr->bodysize));

        for(int i = 0; i < nr->bodysize; i++)
        {
            p1=0;
            for(int j = 0; j < nl->bodysize; j++)
            {
                    res->body[i+j]+=(nr->body[i])*(nl->body[j])+p1;

                    if(p1>0)
                    {
                        p1=0;
                    }
                    if(res->body[i+j]>=BASE)
                    {
                        p1= res->body[i+j]/BASE;
                        res->body[i+j]%=BASE;
                    }
            }
            if(p1>0)
            {
                res->body[i+nl->bodysize]+=p1;
            }
        }
        int newbodsize=0;
        for(int i = res->bodysize-1; i >=0; i--)
        {
            if(res->body[i]!=0)
            {
               newbodsize=i+1;
                break;
            }
        }
        bn *r1 = bn_new();
        bn_allocmemory(r1,newbodsize);
        r1->s=1;
        for(int i =0; i < newbodsize; i++)
        {
            r1->body[i]=res->body[i];
        }
        bn_delete(res);
    bn_delete(nr);
    bn_delete(nl);
    r1->s=1;
    if(left->s==-1||right->s==-1)
        r1->s=-1;
    if(left->s==-1&& right->s==-1)
        r1->s=1;
    return r1;
}
bn* bn_div(bn const *left, bn const *right)
{
    if(bn_check(left)==1 || bn_check(right)==1)
        return NULL;
  if(right->s==0)
    return NULL;
  bn ** arr= (bn**)malloc(10000*sizeof(bn*));
  bn *tp;
  bn *a = bn_init(right);
  a->s=1;
  bn *b = bn_new();
  bn_init_int(b,2);
  int c=0;
  tp = bn_init(left);
  tp->s=1;
  for(int i = 0; bn_cmp(a,tp)<=0; i++)
  {
      c++;
      arr[i]=bn_init(a);
      bn_mul_to(a,b);
  }
  bn_init_int(a,1);
  bn_init_int(b,0);
  for(int i =c-1; i>=0; i--)
  {
          if(bn_cmp(tp,arr[i])>=0)
          {
              bn_sub_to(tp,arr[i]);
              bn *d = bn_new();
              bn_init_int(d,2);
              bn_pow_to(d,i);
              bn_add_to(b,d);
              bn_delete(d);
          }
          bn_delete(arr[i]);
  }
  bn_delete(a);
  free(arr);


  if(b->s!=0)
  {
    if(left->s==1 && right->s==1)
   b->s=1;
    if(left->s==-1 && right->s==-1)
   b->s=1;
    if(left->s==-1 && right->s==1)
    {
       b->s=-1;
       if(tp->s!=0)
       {
            bn *ti=bn_new();
            bn_init_int(ti,1);
            bn_sub_to(b,ti);
            bn_delete(ti);
       }
    }
    if(left->s==1 && right->s==-1)
    {
       b->s=-1;
       if(tp->s!=0)
       {
            bn *ti=bn_new();
            bn_init_int(ti,1);
            bn_sub_to(b,ti);
            bn_delete(ti);
       }
    }
  }
   bn_delete(tp);
  return b;
}
bn* bn_mod(bn const *left, bn const *right)
{
   if(bn_check(left)==1 || bn_check(right)==1)
        return NULL;
   if(right->s==0)
        return NULL;
   if(left->s==0)
   {
       bn *r = bn_new();
       bn_init_int(r,0);
       return r;
   }

  bn ** arr= (bn**)malloc(100000*sizeof(bn*));
  bn ** stepeni= (bn**)malloc(100000*sizeof(bn*));
  bn *tp;
  bn *a = bn_init(right);
  a->s=1;
  bn *b = bn_new();
  bn_init_int(b,2);
  int c=0;
  tp = bn_init(left);
  tp->s=1;
  for(int i = 0; bn_cmp(a,tp)<=0; i++)
  {
      c++;
      arr[i]=bn_init(a);
      bn_mul_to(a,b);
  }
  bn_init_int(a,1);
  for(int i = 0; i < c; i++)
  {
      stepeni[i]=bn_init(a);
      bn_mul_to(a,b);
  }
  bn_delete(a);
 a = bn_init(right);
 bn_delete(b);
  for(int i =c-1; i>=0; i--)
  {
      while(1)
      {
          if(bn_cmp(tp,arr[i])>=0)
          {
              bn_sub_to(tp,arr[i]);
          }
          else
            break;
      }
  }
  for(int i = 0; i < c; i++)
  {
      bn_delete(arr[i]);
      bn_delete(stepeni[i]);
  }
  free(arr);
  free(stepeni);
  a->s=1;
  if(tp->s!=0)
  {
    if(left->s==1 && right->s==1)
    {
        bn_delete(a);

        return tp;
    }
    if(left->s==-1 && right->s==1)
    {
       bn *res = bn_sub(a,tp);
       bn_delete(tp);
       bn_delete(a);
       return res;
    }
    if(left->s==1 && right->s==-1)
    {
       bn *res = bn_sub(a,tp);
       bn_delete(tp);
       bn_neg(res);
       bn_delete(a);
       return res;
    }
    if(left->s==-1 && right->s==-1)
    {
          tp->s=-1;
          bn_delete(a);
          return tp;
    }
  }
  bn_delete(a);
  return tp;
}

const char *bn_to_string(bn const *t, int radix)
{
    if(bn_check(t)==1)
        return NULL;
    if(radix <=1)
        return NULL;
    char *c = (char*)malloc(100020*sizeof(char));
    if(t->s == 0)
    {
        c[0]='0';
        c[1]='\0';
        return c;
    }
    bn *rad = bn_new();
    bn_init_int(rad,radix);
    bn ** arr= (bn**)malloc(100000*sizeof(bn*));
    bn *tp = bn_init(t);
    bn_abs(tp);
    bn* a = bn_new();
    bn_init_int(a,1);
    int c1=0;
    for(int i = 0; bn_cmp(a,tp)<=0; i++)
    {
      c1++;
      arr[i]=bn_init(a);
      bn_mul_to(a,rad);
    }
    int per =0;
    if(t->s==1)
        per=0;
    if(t->s==-1)
    {
        c[0]='-';
        per=1;
    }
    int radc = 0;
    for(int i = c1-1; i>=0; i--)
    {
        radc=0;
        while(bn_cmp(arr[i],tp)<=0)
        {
            bn_sub_to(tp,arr[i]);
            radc++;
        }
        c[c1-1-i+per]=int_to_ch(radc);
    }
  for(int i = 0; i < c1; i++)
  {
    bn_delete(arr[i]);
  }
  free(arr);
  bn_delete(tp);
  bn_delete(a);
  bn_delete(rad);
  c[c1+per]='\0';
  return c;
}
// Если левое мен+ше, вернут+ <0; если равны, вернут+ 0; иначе >0
int bn_cmp(bn const *left, bn const *right)
{
    if(bn_check(left)==1|| bn_check(right)==1)
        return 2;
    if(left->s<right->s)
        return -1;
    if(right->s<left->s)
        return 1;
    if(right->s==0)
        return 0;
    if(left->s>0)
    {
        if(left->bodysize<right->bodysize)
            return -1;
        if(right->bodysize<left->bodysize)
            return 1;
        int ind = 0;

        for(int i = left->bodysize-1; i>=0; i--)
        {
            if(left->body[i]<right->body[i])
            {
                ind = -1;
                break;
            }
            if(left->body[i]>right->body[i])
            {
                ind = 1;
                break;
            }
        }
        return ind;
    }
    if(left->s<0)
    {
        if(left->bodysize<right->bodysize)
            return 1;
        if(right->bodysize<left->bodysize)
            return -1;
        int ind = 0;

        for(int i = left->bodysize; i>=0; i--)
        {
            if(left->body[i]<right->body[i])
            {
                ind = 1;
                break;
            }
            if(left->body[i]>right->body[i])
            {
                ind = -1;
                break;
            }
        }
        return ind;
    }
    return 0;
}
int bn_neg(bn *t)
{
    if(bn_check(t)==1)
        return 1;
    t->s*=-1;
    return 0;
}
int bn_abs(bn *t)
{
  if(bn_check(t)==1)
    return 1;
    if(t->s!=0)
  t->s=1;
  return 0;
}
int bn_sign(bn const *t)
{

    if(bn_check(t)==1)
        return 2;
    if(t->s==-1)
        return -1;
    if(t->s==1)
        return 1;
    return 0;
}

int main()
{
    bn* n = bn_new();
    bn*old2 = bn_new();
    bn*old1 = bn_new();
    bn_init_int(old1,1);
    bn_init_int(old2,1);
    int n1;
    scanf("%d",&n1);
    for(int i =0; i < n1-2; i++)
    {
       bn_delete(n);
       n=bn_add(old1,old2);
       bn_copy(old2,old1);
       bn_copy(old1,n);
    }
    bn_delete(old1);
    bn_delete(old2);
    const char *c = bn_to_string(n,10);
    bn_delete(n);
    printf("%s",c);
    free((char*)c);

}


OBJS = main.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSSC = main-sc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSMUTSC = main-mut-sc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSALLSC = main-all-sc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSALLFSMSC = main-all-fsm-sc.o FSM.o Graph.o Statistics.o BigNumber.o
OBJSRNDFSMSC = main-rnd-fsm-sc.o FSM.o Graph.o Statistics.o BigNumber.o
OBJSMUTDISTSC = main-mut-dist-sc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSALLKMUTSC = main-all-k-mut-sc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSKMLSC = main-k-ml-sc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSSCLENGTH = main-sc-length.o FSM.o Graph.o BigNumber.o
OBJSSCMETODOS = main-sc-metodos.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSRNDFSMTCRSC2 = main-rnd-fsm-tc-rsc2.o FSM.o Graph.o Statistics.o BigNumber.o
OBJSRNDFSMTCRSC = main-rnd-fsm-tc-rsc.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJSRNDFSMTCRSCFINAL = main-rnd-fsm-tc-rsc-FINAL.o FSM.o Graph.o Statistics.o BigNumber.o
OBJGERAFSM = main-gera-fsm.o FSM.o Graph.o Statistics.o BigNumber.o
OBJGERAUMAFSM = main-gera-uma-fsm.o FSM.o Graph.o Statistics.o BigNumber.o
OBJSSCARQ = main-sc-arq.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o
OBJEHREDUZIDA = main-is-reduced.o FSM.o FSM.tab.o lex.yy.o Graph.o Statistics.o BigNumber.o

LIBS = -lstdc++ 

LIBPATH =

INCLUDEPATH =

GCC = gcc -g -Wall

all : sc fsm-sc fsm-sc-metodos fsm-mut-sc fsm-mut-dist-sc fsm-all-sc fsm-all-k-mut-sc fsm-k-ml-sc fsm-all-fsm-sc fsm-rnd-fsm-tc-rsc2 fsm-rnd-fsm-tc-rsc fsm-rnd-fsm-sc fsm-rnd-fsm-tc-rsc-FINAL fsm-gera-fsm fsm-gera-uma-fsm fsm-sc-arq fsm-is-reduced

sc : $(OBJS)
	$(GCC) $(LIBS) $(LIBPATH) -o sc $(OBJS)

fsm-sc : $(OBJSSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-sc $(OBJSSC)

fsm-mut-dist-sc : $(OBJSMUTDISTSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-mut-dist-sc $(OBJSMUTDISTSC)

fsm-mut-sc : $(OBJSMUTSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-mut-sc $(OBJSMUTSC)

fsm-all-sc : $(OBJSALLSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-all-sc $(OBJSALLSC)

fsm-all-k-mut-sc : $(OBJSALLKMUTSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-all-k-mut-sc $(OBJSALLKMUTSC)

fsm-k-ml-sc : $(OBJSKMLSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-k-ml-sc $(OBJSKMLSC)

fsm-all-fsm-sc : $(OBJSALLFSMSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-all-fsm-sc $(OBJSALLFSMSC)

sc-length : $(OBJSSCLENGTH)
	$(GCC) $(LIBS) $(LIBPATH) -o sc-length $(OBJSSCLENGTH)

fsm-rnd-fsm-sc : $(OBJSRNDFSMSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-rnd-fsm-sc $(OBJSRNDFSMSC)

fsm-sc-metodos : $(OBJSSCMETODOS)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-sc-metodos $(OBJSSCMETODOS)

fsm-rnd-fsm-tc-rsc : $(OBJSRNDFSMTCRSC)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-rnd-fsm-tc-rsc $(OBJSRNDFSMTCRSC)

fsm-rnd-fsm-tc-rsc2 : $(OBJSRNDFSMTCRSC2)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-rnd-fsm-tc-rsc2 $(OBJSRNDFSMTCRSC2)

fsm-rnd-fsm-tc-rsc-FINAL : $(OBJSRNDFSMTCRSCFINAL)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-rnd-fsm-tc-rsc-FINAL $(OBJSRNDFSMTCRSCFINAL)

fsm-gera-fsm : $(OBJGERAFSM)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-gera-fsm $(OBJGERAFSM)

fsm-gera-uma-fsm : $(OBJGERAUMAFSM)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-gera-uma-fsm $(OBJGERAUMAFSM)

fsm-sc-arq : $(OBJSSCARQ)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-sc-arq $(OBJSSCARQ)

fsm-is-reduced : $(OBJEHREDUZIDA)
	$(GCC) $(LIBS) $(LIBPATH) -o fsm-is-reduced $(OBJEHREDUZIDA)

%.o : %.cpp %.h
	$(GCC) $(INCLUDEPATH) -o $@ -c $<

Graph-verb.o : Graph.cpp Graph.h
	$(GCC) $(INCLUDEPATH) -DVERBOSE -o Graph-verb.o -c Graph.cpp

FSM.tab.cpp : FSM.y
	bison -o FSM.tab.cpp -d $<

lex.yy.cpp : FSM.l
	flex -o lex.yy.cpp FSM.l

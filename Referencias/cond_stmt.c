//Implementação da produção <cond_stmt> gerando código MEPA
//<cond_stmt> ::= if ‘(‘ <expr> ‘)’ <stmt> [ else <stmt> ]
void cond_smt(){
    consome(if);
    consome(abre_par);
    expr(); // E
    consome(fecha_par);
    printf("DSVF L1");
    stmt(); // C1
    printf("DSVS L2");
    printf("L1:NADA");
    if( lookahead == else){
        consome (else);
        stmt(); // C2
        
    }
    printf("L2:NADA");


}
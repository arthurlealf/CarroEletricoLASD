# CarroEletricoLASD
Projeto desenvolvido na disciplina de Laboratório de Arquitetura de Sistemas Digitais - Carro Elétrico


Aluno: Arthur de Farias Leal - Turma 7

DESCRIÇÃO DA SPRINT 10.

Foram implementadas duas novas funcionalidades para o carro elétrico. São elas:

1. SISTEMA DE ALERTA DE AQUECIMENTO
2. FAROL

Descrições:

1. SISTEMA DE ALERTA DE AQUECIMENTO:

Esse sistema é responsável por indicar quando há um determinado nível de aquecimento no carro (150° no caso
desse projeto).
Para comçear, foi implementada a condição para aquecimento. Se (if) a temperatura ultrapassar 150°, a porta
C3 - onde está conectado o auto-falante - é habilitada e um sinal sonoro é emitido. Caso contrário (temp<150),
nada acontece. 
No montagem foi colocado um amplificador ligado ao auto-falante, pois o sinal sonoro estava muito baixo. 
Para incrementar esse sistema de alarme, também foi implementada uma mensagem de alerta (ALERTA DE AQUECIMENTO!!!)
no LCD. Ela foi implementada a partir dos recursos oferecidos pela biblioteca do LCD.


2. FAROL:

Esse funcionalidade foi projetada com a utilização de LED's (representando o farol) e o monitor serial (responsável
por receber os comandos para ligar e desligar o farol).
Para isso, foram criadas duas condições: 
	Se (if) o comando informado no serial for 'f', a porta C6 é habilitada e o farol liga.
	Se (if) o comando informado no serial for 's', o farol desliga.
	

CORREÇÃO: No vídeo - minuto 03:28 - eu falei que o farol estava conectado na porta C3, quando ele está na porta C6. 
Foi apenas uma falha.. ao longo do vídeo eu falo que ele está conectado na porta correta (C6)

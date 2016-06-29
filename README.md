## Introducción

Para este proyecto seleccionamos uno de los 3 problemas del *Super Computing and Distributed Systems Camp Programming Contest* del año 2012, en este campamentos se suele evaluar el desempeño de los programas paralelos de los concursantes mediante la fórmula de aceleración que vimos en clases $S=\dfrac{Tiempo Secuencial}{Tiempo Paralelo}$  asdasdasd

## Bombing field

### Planteamiento del problema

Párrafo divertido

* Lista no ord
* Con 
* cosas 

1. Primer elemento
1. Segundo
1. Y tercero

### Solución propuesta

Párrafo más divertido

## Weird puzzle

### Planteamiento del problema

Este problema consiste en una matriz cuadrada de tamaño N compuesta de caracteres y un conjunto
de palabras que pueden estar o no en la matriz. La idea es calcular cuántas de estas palaras se
encuentran en esta matriz, tomando en cuenta que las letras de la palabra no tienen que estar
necesariamente contiguas, pueden comenzar al final y seguir al princpio así como también estar
en reversa

### Solución propuesta

#### Paralelización

Para alcanzar la solución deseada aprovechando el paralelismo ofrecido por un conjunto de máquinas
se decidió como mejor manera paralelizar el cálculo sobre las filas y columnas de la matriz, de
manera que cada proceso calcule sobre una parte de la matriz que no comparta con ningún otro proceso
Cada proceso recibirá también la lista completa de palabras para ser buscadas entre las filas y columnas
que haya recibido.

#### Repartición del trabajo

Para alcanzar el objetivo se propone repartir tanto las filas como las columnas de la matriz entre
los diferentes procesos en ejecución. Se programará un proceso maestro en de manera que reparta las
filas y columnas de la matriz haciendo uso de la primitiva MPI_scatter de la forma más balanceada
posible. Luego este mismo proceso enviará mediante un MPI_Broadcast la lista de palabras para que
todos los procesos la reciban completa.

#### Cálculo paralelo para cada proceso

Una vez recibida la lista de filas y columnas y la lista de palabras el proceso generará un arreglo de bits
de tamaño W, este arreglo de bits representará el conjunto de palabras donde 1 significa que la palara fue
encontrada en la matriz y 0 significa lo contrario. Posteriormente, el proceso iterará sobre la lista
de filas y columnas donde para cada una se generará un arreglo de tamaño 2*N que será la fila o columna
dos veces de manera continua, una vez con este arreglo para cada palabra de la lista de palabras se
verifcicará que en el arreglo se encuentren la totalidad de las letras en el orden que muestran en la palabra
actual. La misma verificación se hará también pero con el orden inverso del arreglo mencionado. De ser verificado
lo mencionado anteriormente, para esta palabra se colocará un 1 en el arreglo de bits en el índice correspondiente
al número de la palabra. Una vez iterada cada lista de palabras en cada lista de filas o columnas el proceso tendrá
el cálculo parcial en el arreglo de bits mencionado al principio de este párrafo.

#### Totalización del cálculo

Cada proceso al finalizar el cálculo retornará el arreglo de bits haciendo uso de la primitiva MPI_Gather al
proceso maestro. El proceso maestro realizará un AND bit a bit entre todos los arreglos retornados por los
procesos y finalmente devolverá el número de unos que existan en el arreglo.

	

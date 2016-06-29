# Introducción

Para este proyecto seleccionamos uno de los 3 problemas del *Super Computing and Distributed Systems Camp Programming Contest* del año 2012, en este campamentos se suele evaluar el desempeño de los programas paralelos de los concursantes mediante la fórmula de aceleración que presentado en clases $(A=\dfrac{Tiempo Secuencial}{Tiempo Paralelo})$ es por esto que como primer paso implementamos un programa secuencial que resolviera el problema planteado.


# Planteamiento del problema

Se presenta un campo de batalla de tamaño NxN en el cual se encuentran objetivos militares y civiles los culaes poseen un valor que indica su resistencia, adicionalmente existe una serie de bombas las cuales actúan sobre áreas cuadradas con cierta potencia atacando a los diferentes objetivos. El problema a resolver es el cálculo de la cantidad de objetivos militares que han sido destruidos, afectados parcialmente y destruidos, de igual forma con los objetivos civiles.

Los datos del programa serán recibidos por entrada estándar.

# Solución propuesta

## Análisis de solución secuencial

Existen varias soluciones "inocentes" que existen para atacar este problema, la primera es intentar observar el campo de batalla como una matriz en la cual los objetivos estan realmente distribuidos, pero sería un gran desperdicio de espacio a menos que la cantidad de objetivos se aproximadamente N $\times$ N. 

Partiendo del hecho de que nos encontramos frente a una matriz *sparse* se procedimos a la siguiente toma de decisiones. Para procesar los ataques se decidió iterar sobre las bombas por cada *target* posible, ya que hacerlo en orden inverso es más complicado en complejidad y espacio determinar el estado final al terminar el bombardeo.

Adicionalmente se decidió realizar un preprocesamiento del arreglo de bombas, ya que el formato proporcionado (radio cuadrado y centro) obliga a calcular los límites del área con cada ataque, pero puede ser evitado relizando el cálculo una única vez y con cada ataque solo se deberá revisar si un objetivo en encuentra entre los puntos (x0,y0) y (x1,y1) del mismo.

El flujo para la resolución del problema puede ser divido de forma secuencial en el siguiente algoritmo en pseudo-código:

* Procesar entrada
* Realizar conversión de formato de bomba
* Por cada *objetivo*
    - Verificar el tipo de objetivo (militar/civil)
    - Marcar como no atacado
    - Por cada bomba
        + Si el objetivo está en el área
            * Marcar como atacado
            * Actualizar su *resistencia*
    - Actualizar los contadores de targets afectados, destruidos o atacados según sea el caso

Este algoritmo fue implementado en el archivo *secuencial.c*.

## Paralelización

### Repartición del trabajo

Primero, analizamos las diferentes formas de dividir el problemas y optamos por un particionamientos de datos (descomposición de dominios), ya que el algoritmo 


# Análisis de otros problemas


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

## Yet more primes

### Planteamiento del problema

turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles

### Solución propuesta

turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles

#### Paralelización

turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles

#### Repartición del trabajo

turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles

#### Cálculo paralelo para cada proceso

turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles

#### Totalización del cálculo

turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles lots of turtles

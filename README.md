---
header-includes: \usepackage{graphicx}
---
\newpage


# Introducción

Para este proyecto seleccionamos uno de los 3 problemas del *Super Computing and Distributed Systems Camp Programming Contest* del año 2012, en este campamentos se suele evaluar el desempeño de los programas paralelos de los concursantes mediante la fórmula de aceleración que presentado en clases $(A=\dfrac{Tiempo Secuencial}{Tiempo Paralelo})$ es por esto que como primer paso implementamos un programa secuencial que resolviera el problema planteado.


# Planteamiento del problema

Se presenta un campo de batalla de tamaño NxN en el cual se encuentran objetivos militares y civiles los cuales poseen un valor que indica su resistencia, adicionalmente existe una serie de bombas las cuales actúan sobre áreas cuadradas con cierta potencia atacando a los diferentes objetivos. El problema a resolver es el cálculo de la cantidad de objetivos militares que han sido destruidos, afectados parcialmente y destruidos, de igual forma con los objetivos civiles.

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
    * Verificar el tipo de objetivo (militar/civil)
    * Marcar como no atacado
    * Por cada bomba
        * Si el objetivo está en el área
            * Marcar como atacado
            * Actualizar su *resistencia*
    * Actualizar los contadores de targets afectados, destruidos o atacados según sea el caso

Este algoritmo fue implementado en el archivo *secuencial.c*.

## Paralelización

### Repartición del trabajo

Primero, analizamos las diferentes formas de dividir el problemas y optamos por un particionamientos de datos (descomposición de dominios), primero observarmos que el cálculo del estado de cada *target* es completamente independiente de los otros por lo cual presenta un buen escenario para la paralelización. Además la cantidad de targets se puede repartir de forma relativamente homogénea dividiendo la cantidad de casos entre la cantidad de procesos.

Para nuestra implementación decidimos utilizar MPI_Scatter para enviar a todos los nodos la cantidad de *targets que deben procesar* (world_size / n_targets) mientras que el nodo raiz procesara su segmento y el excedente (world_size % n_targets).

### Cálculo paralelo de cada trabajo

Cada proceso realizará el cálculo de objetivos destruidos, parcialmente destruidos y no atacados de forma secuencial sobre su conjuntos de datos el cual irá acumulado en un vector de resultado. 

### Totalización del cálculo

Luego de que cada nodo terminó de realizar su procesamiento, mediante el uso del procedimiento MPI_Gather, se acumularán todos los vectores resultantes en el nodo maestro, el cual procederá a reducir todas las respuestas a un solo arreglo de tamaño 6 (3 para civiles, y 3 para militares) mediante la sumatoria de las posiciones afines.

### Tablas de análisis

#### Secuencial
| Tamaño de entrada                  |Tiempo    (uSec)|
|------------------------------------|----------------|
|20000 objetivos, 1000 bombas        | 0:59563        |
|400000 objetivos, 2000 bombas       | 2:347187       |
|800000 objetivos, 8000 bombas       | 18:708761      |
|1000000 objetivos, 10000 bombas     | 29:216888      |

#### 2 procesos
| Tamaño de entrada (nro de targets)    |Tiempo secuencial (uSec)|Tiempo paralelo (uSec)| Aceleración |
|---------------------------------------|------------------------|----------------------|-------------|
|20000 objetivos, 1000 bombas           | 0:59563                | 0:30935              | 1.9254      |
|400000 objetivos, 2000 bombas          | 2:347187               | 1:179404             | 1.9901      |
|800000 objetivos, 8000 bombas          | 18:708761              | 9:369040             | 1.9968      |
|1000000 objetivos, 10000 bombas        | 29:216888              | 14:626805            | 1.9974      |

#### 4 procesos
| Tamaño de entrada (nro de targets)    |Tiempo secuencial (uSec)|Tiempo paralelo (uSec)| Aceleración |
|---------------------------------------|------------------------|----------------------|-------------|
|20000 objetivos, 1000 bombas           | 0:59563                | 0:17390              | 3.4251      |
|400000 objetivos, 2000 bombas          | 2:347187               | 0:614890             | 3.8172      |
|800000 objetivos, 8000 bombas          | 18:708761              | 4:736750             | 3.9497      |
|1000000 objetivos, 10000 bombas        | 29:216888              | 7:378487             | 3.9597      |

#### 8 procesos
| Tamaño de entrada (nro de targets)    |Tiempo secuencial (uSec)|Tiempo paralelo (uSec)| Aceleración |
|---------------------------------------|------------------------|----------------------|-------------|
|20000 objetivos, 1000 bombas           | 0:59563                | 0:12257              | 4.8595      |
|400000 objetivos, 2000 bombas          | 2:347187               | 0:334091             | 7.0255      |
|800000 objetivos, 8000 bombas          | 18:708761              | 2:418354             | 7.7361      |
|1000000 objetivos, 10000 bombas        | 29:216888              | 3:751257             | 7.7885      |


# Análisis de otros problemas


## Weird puzzle

### Planteamiento del problema

El problema **Weird puzzle** consiste en una matriz de tamaño **N** compuesta de caracteres y una lista
de **W** palabras que pueden estar o no en la matriz. La idea es calcular cuántas de estas palaras se
encuentran en esta matriz, tomando en cuenta que las letras de la palabra no tienen que estar
necesariamente contiguas, pueden comenzar al final y seguir al principio así como también estar
en reversa.

### Solución propuesta

#### Paralelización

Para alcanzar la solución deseada aprovechando el paralelismo ofrecido por un conjunto de máquinas
se decidió como mejor manera paralelizar el cálculo sobre las filas y columnas de la matriz, de
manera que cada proceso realice su parte del cómputo sobre una región de la matriz que no comparta con ningún otro proceso. 
Cada proceso recibirá también la lista completa de palabras para ser buscadas entre las filas y columnas
que haya recibido.

#### Repartición del trabajo

Para alcanzar el objetivo se propone repartir tanto las filas como las columnas de la matriz entre
los diferentes procesos en ejecución. Se programará un proceso maestro de manera que reparta las
filas y columnas de la matriz haciendo uso de la primitiva **MPI_scatter** de la forma más balanceada
posible. Luego este mismo proceso enviará mediante un **MPI_Broadcast** la lista de palabras para que
todos los procesos la reciban completa.

#### Cálculo paralelo para cada proceso

Una vez recibida la lista de filas y columnas y la lista de palabras el proceso generará un arreglo de bits
de tamaño W. Este arreglo de bits representará el conjunto de palabras donde 1 significa que la palabra fue
encontrada en la matriz y 0 significa lo contrario. Posteriormente, el proceso iterará sobre la lista
de filas y columnas donde para cada una se ellas generará un arreglo de tamaño **2*N** el cual será la fila o columna
dos veces de manera continua. Ya con este arreglo, para cada palabra de la lista de se
verifcicará que en el arreglo se encuentren la totalidad de las letras de esta en el orden que se observa en la palabra
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

Para este problema se tiene una secuencia de "medios números" los cuales deben ser concatenados para probar posteriormente si son primos o no. 

Además, potencialmente hay números mal escritos (empezando en 0), lo cual debe ser tomado en cuenta si se desea ahorrar tiempo de computo.

Es importante notar que el cálculo de cada número concatenado es independiente de los demás.


Por último se debe notar que es requerido ordenar los resultados.

### Solución propuesta

Esto se puede realizar de forma secuencial en un ciclo en el cual cada string es concatenados con todos los demás y luego se procede a intentar convertir este nuevo string en a un número (teniendo cuidado de la longitud y la variable de almacenamiento), posteriormente se debe proceder a realizar la verificación normal de si un número es primo o no, verificando primero si es par o no, en caso de que no lo sea proceder a realizar las verificación de n % i para los números impares entre 3 y $\sqrt{n}$.

La cantidad de números a probar viene dado por N $\times$ (N-1), en el cual cada string es concatenado con todos los demás a probar.

Como no es sencillos ordenar a priori todos los medios números, solo por su longitud y además es algo costoso ordenar elementos que puedes no necesitar, se puede ejecutar un algoritmo de ordenamiento sobre el arreglo de resultados. En el caso secuencial se puede utilizar quicksort por cuestiones de eficiencia.

#### Paralelización

Los cálculos pertinentes para cada número formado, como ya se dijo son disjuntos, esta independencia de datos permite que, teniendo todos los strings en cada nodo, se pueda realizar una división equitativa del trabajo, tomando como trabajo principal para cada nodo n strings como lados izquierdos de medios números, y los restantes como lados derechos.

Con respecto al orden que debe llevar el resultado final, se pueden tomar diferentes aproximaciones. En presencia de cantidades lo suficientemente pequeñas de números primos resultantes, se podría realizar un quicksort en el nodo maestro sin muchos problemas, pero para n lo suficientemente grandes, sería buena idea utilizar algoritmos que no trabajen con movimiento de arreglos en sitio, sino con algunos similares a mergesort.

#### Repartición del trabajo

Para este problema, se puede realizar un MPI_Broadcast para que todos los nodos reciban el arreglo completo de strings, adicionalmente se puede enviar el rango de strings que deben probar como lados izquierdos (arreglo de dos enteros indicando linea inicial y final).

Por ejemplo un nodo que le toque el primer string deberá revisar todas las concatenaciones entre este y los demás strings, por otra parte las permutaciones de estos casos las realizarán los otros nodos de forma independiente tomando sus respectivos strings como lados izquierdo, el primer string como lado derecho.

#### Cálculo paralelo para cada proceso

Cada proceso debe realizar los mismos cálculos del caso sencuencial: verificar que el número esta bien escrito, concatenar el strings que encuentra procesando con todos los demás y verificar eficientemente si es primo o no.

Posteriormente cada nodo deberá indicar la cantidad de números primos que econtró y el arreglo con los mismos.

#### Totalización del cálculo

La totalización de calculos debe ser mediante envíos individuales hacia en nodo raíz, el cual itera esperando mensajes de cada nodo, primero uno indicando la cantidad de elementos que va a recibir y luego el arreglo. Posteriormente, en el caso sencillo el nodo raíz ordena los resultados recibidos.

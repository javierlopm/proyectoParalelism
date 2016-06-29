# Introducción

Para este proyecto seleccionamos uno de los 3 problemas del *Super Computing and Distributed Systems Camp Programming Contest* del año 2012, en este campamentos se suele evaluar el desempeño de los programas paralelos de los concursantes mediante la fórmula de aceleración que presentado en clases $(A=\dfrac{Tiempo Secuencial}{Tiempo Paralelo})$ es por esto que como primer paso implementamos un programa secuencial que resolviera el problema planteado.



# Planteamiento del problema

Se presenta un campo de batalla de tamaño NxN en el cual se encuentran objetivos militares y civiles los culaes poseen un valor que indica su resistencia, adicionalmente existe una serie de bombas las cuales actúan sobre áreas cuadradas con cierta potencia atacando a los diferentes objetivos. El problema a resolver es el cálculo de la cantidad de objetivos militares que han sido destruidos, afectados parcialmente y destruidos, de igual forma con los objetivos civiles.

Los datos del programa serán recibidos por entrada estándar.

# Solución propuesta

Cosas aquí para introducir este punto

Existen varias soluciones "inocentes" que existen para atacar este problema, la primera es intentar observar el campo de batalla como una matriz en la cual los objetivos estan realmente distribuidos, pero sería un gran desperdicio de espacio a menos que la cantidad de objetivos se aproximadamente N \times N. 

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

A partir de esta implementación se inició la construcción del programa paralelo.

Primero, analizamos las diferentes formas de dividir el problemas y optamos por un particionamientos de datos (descomposición de dominios), ya que el algoritmo 

# Análisis de otros problemas
-
## Weird Puzzle

## Yet More Primes



# DEVLOG - Visualizador ADS-B
Autor: Francisco Medinilla García

El objetivo de este documento es compartir detalles sobre el proceso de desarrollo, reflexiones, decesiones técnicas, aprendizaje, problemas encontrados y soluciones implementadas.

La idea es documentar el viaje de desarrollo más allá de los resultados finales.

## 03 de julio de 2025 - Trabajando con flight_t

**OBJETIVOS**

Completar el tipo flight_t

**DETALLES TECNICOS**

Queremos que los vuelos (flight_t) tengan: ICAO, callsign, latitude, longitude, altitude, speed, track.

- ICAO: Número identificativo de la aeronave (3 bytes).
- callsign: Nombre identificativo (8 caracteres) para comunicaciones por radio.
- latitude: Distancia hacia Norte - Sur [-90 (sur), 90 (norte)]. Valor decimal.
- longitude: Distancia hacia Este - Oeste [-180 (oeste), 180 (este)]. Valor decimal.
- altitude: Altura barométrica de la aeronave, medido en pies. Valor entero.
- speed: Velocidad lineal de la aeroanve, medida en nudos. Valor entero.
- track: Grado que marca la dirección de la aeronave, con referencia al norte (0º). Valor decimal

**OBSERVACIONES**

La aproximación programada para el recalculo de las coordenadas no está comprobado.

**RESULTADOS**

- Tipo flight_t completo
- Aproximación del método flight_update_coordinates (a falta de comprobar)

**PROXIMOS PASOS**

- Empezar con la codificación de mensajes ADS-B
- Agregar la capacidad de poder mandar diferentes tipos de mensaje (identificacion, posicion, etc.)

## 30 de junio de 2025 - Generador de datos ficticios

**OBJETIVO**

Tener una estructura para el generador de datos. Empezar a generar datos ficticios (sin tener en cuenta el formato ADS-B).

**DETALLES TECNICOS**

La idea principal es crear un proceso principal que creará tantos hilos como aeronaves se quiera generar. Cada hilo generado tendrá los datos de una aeronave y mandará un mensaje cada segundo (esto puede variar mas adelante). 

Antes de enviar el mensaje, el hilo de la aeronave actualizará los datos de posición.

**OBSERVACIONES**

La necesidad de tener codigo compartido entre generator/ y console/ se ha solucionado creando un directorio shared/ y en los Makefiles hacer las modificaciones oportunas para el correcto uso de esta carpeta.

**RESULTADOS**
- Se crea el fichero .gitignore para ignorar los directorios build/ y bin/
- Se agrega un target a los Makefile para ejecutar cada aplicación
- Se crea directorio shared/ en la raiz para codigo que se necesite compartir entre console/ y generator/
- Se crea el esqueleto basico de los vuelos (aeronaves) en shared/
- Se crea la función para iniciar el generador (***generator_start***)

**PROXIMOS PASOS**
- Completar los atributos del tipo ***flight_t***
- Implementar una aproximación más realista de ***flight_update_coordinates***

## 28 de junio de 2025 - Configuración básica del proyecto

**OBJETIVO**

Tener el esqueleto principal para empezar el desarrollo del proyecto.

**OBSERVACIONES**

Para ejecutar correctamente make se tiene que usar una consola Unix.

Si se utiliza la consola de windows puede haber fallos dado que los comandos utilizados en el Makefile no son compatibles con windows. Desde windows se puede utilizar GitBash o WSL (o similar).

**RESULTADOS**

- Repositorio de GIT inicializado
- Fichero README con la descripción del proyecto
- Fichero LICENCE con la licencia MIT
- Fichero DEVLOG
- Esqueleto básico para la consola en el directorio console/
- Esqueleto básico para el generador de datos en el directorio generator/

**PROXIMOS PASOS**

- Estructurar generador de datos
- Empezar desarrollo del generador de datos
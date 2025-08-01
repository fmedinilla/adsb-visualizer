# DEVLOG - Visualizador ADS-B
Autor: Francisco Medinilla García

El objetivo de este documento es compartir detalles sobre el proceso de desarrollo, reflexiones, decesiones técnicas, aprendizaje, problemas encontrados y soluciones implementadas.

La idea es documentar el viaje de desarrollo más allá de los resultados finales.

## 01 de agosto de 2025 - Mensaje de la posición de la aeronave

**OBJETIVOS**

Empezar con la implementación de la codificación de los mensajes de posición de la aeronave.

**DETALLES TECNICOS**

Para implementar la logica del método ***flight_update_coordinates*** vamos a dividirlo en seis partes: las constantes de cálculo, la conversión de unidades, los cálculos propiamente dichos, y la actualización de las coordenadas.

Necesitamos 4 constantes:
- EARTH_RADIUS: es el radio de la tierra en metros.
- KNOTS_TO_MPS: es el factor para pasar de nudos a metros por segundo.
- DEG_TO_RAD: es el factor para pasar de grados a radianes.
- RAD_TO_DEG: es el factor para pasar de radianes a grados.

Aplicamos las fórmulas de cálculo de la nuevas coordenadas:

$$
\varphi_2 = \arcsin\left( \sin(\varphi_1) \cdot \cos\left(\frac{d}{R}\right) + \cos(\varphi_1) \cdot \sin\left(\frac{d}{R}\right) \cdot \cos(\theta) \right)
$$

$$
\lambda_2 = \lambda_1 + \arctan2\left( \sin(\theta) \cdot \sin\left(\frac{d}{R}\right) \cdot \cos(\varphi_1), \cos\left(\frac{d}{R}\right) - \sin(\varphi_1) \cdot \sin(\varphi_2) \right)
$$

Donde:

- φ₁, λ₁: latitud y longitud inicial (en radianes)  
- φ₂, λ₂: latitud y longitud final (en radianes)  
- θ: rumbo (track) en radianes, medido desde el norte hacia el este  
- d: distancia recorrida en metros  
- R: radio de la Tierra (≈ 6,371,000 m)

Para la codificación de la posición un mensaje se divide en: TC(5) - SS(2) - SAF(1) - ALT(12) - T(1) - F(1) - LAT_CPR(17) - LON_CPR(17).

Donde: 
- TC: es el codigo del tipo de mensaje, aqui se indica el tipo del altura que se envia, que puede ser altitud barometrica o altitud GNSS. Por simplificación usaremos la altitud GNSS, es decir, nuestro TC siempre será 20.
- SS: es el estado de vigilancia, por simplificación, será siempre 0.
- SAF: es Single antenna flag, por simplificación, será siempre 0.
- ALT: numero decimal codificado en 12 bits. Es la altura en metros.
- T: es una bandera del tiempo, será 0 por simplificación.
- F: Es el formato CPR, puede ser 0 (even) o 1 (odd).
- LAT_CPR: Es la latitud codificada usando CPR.
- LON_CPR: Es la longitud codificada usando CPR.

**OBSERVACIONES**

El método ***adsb_set_position_me*** tiene una flag interna que dice si el mensaje es even u odd. Esta flag deberá autocalcularse ya que se deben mandar mensajes intercambiando esta flag (even, odd, even, odd, etc.).

**RESULTADOS**

- Método ***flight_update_coordinates*** con una aproximación real.
- Método ***adsb_set_position_me*** implementado a falta de la flag is_even.

**PROXIMOS PASOS**

- Agregar el calculo de la flag is_even del método ***adsb_set_position_me***.

## 19 de julio de 2025 - CRC para la paridad del frame ADS-B

**OBJETIVOS**

Implementar el control de errores de los frame ADS-B.

**DETALLES TECNICOS**

Las comunicaciones Mode S (incluyendo ADS-B) utilizan codificación de control de errores CRC. En la página del [mode-s](https://mode-s.org/1090mhz/content/ads-b/8-error-control.html) aparece como hacer este control de errores. Se va a implementar el mismo código que aparece en dicha web.

**OBSERVACIONES**

Se han borrado los metodos ***adsb_identificaction_get_charpos*** y ***adsb_identificaction_get_char*** del fichero adsb.h, ya que estos métodos no deberían ser usados por nadie más que los propios métodos de codificación y decodificación de adsb. La implementación se mantiene en adsb.c para poder ser usados.

**RESULTADOS**

- Diferentes funciones para construir frames ADS-B completos.
- Refactorización del método ***flight_get_message***, este método ya no sabe como es la codificación ADS-B, simplemente llama a funciones que construyen el mensaje completo.
- Implementación de CRC para el cálculo de los bytes de paridad del frame ADS-B.
- Se ha agregado la capacidad de enviar diferentes tipos de mensajes, que se quitó el día 15 de julio de 2025.

**PROXIMOS PASOS**

- Trabajar con la implementación del metodo ***adsb_set_position_me*** para codificar mensajes de la posición de la aeronave.
- Rediseñar el calculo de nueva posición de la aeronave, el método ***flight_update_coordinates***.

## 15 de julio de 2025 - Codificación frame ADS-B

**OBJETIVOS**

Codificar mensajes de identificación y obtener el frame ADS-B completo de dicho mensaje.

**DETALLES TECNICOS**

Un mensaje (ME) de identificación en ADS-B tiene la siguiente estructura:

| TC,5 | CA,3 | C1,6 | C2,6 | C3,6 | C4,6 | C5,6 | C6,6 | C7,6 | C8,6 |

Donde TC es el Type Code, CA es Aircraft Category, y C* es un caracter del callsign.

Los caracteres son valores decimales que se corresponden a una posición de la siguiente cadena: #ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######

- A - Z :   1 - 26
- 0 - 9 :  48 - 57
- ␣ :  32

Los parametros TC y CA en conjunto dan la información sobre la categoria de turbulencia de la aeronave, para simplificar usaremos siempre TC = 4 y CA = 2. Esto significa que es una aeronave de tipo Medium 1 (7 000 Kg a 34 0000 Kg).

**OBSERVACIONES**

Se ha modificado y limpiado la mayor parte del codigo escrito el ultimo dia, con la finalidad de limpiar y tener una mayor legibilidad del codigo. También, se ha perdido la capacidad de enviar diferentes tipos de mensajes.

**RESULTADOS**
- Simulación de vuelo independiente de la codificación ADSB.
- Dos metodos para la codificacion y decodificación del callsign.
- Metodo para hacer el mensaje ADSB.

**PROXIMOS PASOS**
- Implementar control de error (PI) para los mensajes.
- Refactorizar ***flight_get_message***, este metodo no debería saber como se codifica el mensaje.

## 11 de julio de 2025 - Primeros pasos en la codificación ADS-B

**OBJETIVOS**

Empezar con la codificación de mensajes ADSB.

**DETALLES TECNICOS**

Para permitir el envio de distintos tipos de mensaje se crea un enum llamado ***message_type_t***, con dos valores IDENTIFICATION_MESSAGE y POSITION_MESSAGE. Dentro de la estructura ***flight_t*** se agrega un campo nuevo de tipo ***message_type_t***. Al mandar un mensaje se comprueba que ***message_type_t*** hay en el vuelo, se manda el mensaje correspondiente y se cambia el tipo de mensaje que el siguiente mensaje sea diferente.

La estructura de un frame ADSB es la siguiente: DF (5) - CA (3) - ICAO (24) - ME (56) - PI (24), esto significa que DF (Downlink Format) son 5 bits, CA (Transponder Capacity) son 3 bits, ICAO son 24 bits, ME (Message) son 54 bits y PI (Parity) son 24 bits. Esto hace un total de 112 bits (14 bytes).

Para simplificar el proyecto, las aeronaves generadas tendran un DF de 17, lo que significa que son aeronaves civiles y con una capacidad de transponder de 5, lo que significa un nivel 2+ de transponder. Por lo tanto todos los ADSB Frame que se generen en la aplicación empezaran por: 17 - 5 (o en binario 10001 - 101; o en hexadecimal 8D)

Antes de mandar cada mensaje, se creará el frame adsb y posteriormente se enviara un mensaje. Este mensaje será el frame ADSB en hexadecimal (un total de 28 caracteres hexadecimales, que hacen los 14 bytes totales).

**OBSERVACIONES**


**RESULTADOS**
- Se ha creado el tipo ***adsb_frame_t***
- Se ha creado el tipo ***adsb_message_t***
- Se ha agregado la capacidad de enviar diferentes tipos de mensajes

**PROXIMOS PASOS**
- Codificar mensajes del tipo identificacion y agregarlos al frame

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
# DEVLOG - Visualizador ADS-B
Autor: Francisco Medinilla García

El objetivo de este documento es compartir detalles sobre el proceso de desarrollo, reflexiones, decesiones técnicas, aprendizaje, problemas encontrados y soluciones implementadas.

La idea es documentar el viaje de desarrollo más allá de los resultados finales.

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
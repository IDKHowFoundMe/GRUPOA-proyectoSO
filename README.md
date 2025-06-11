# GRUPOA-proyectoSO


# Proyecto: Ejecución de múltiples tareas con procesos y con hilos

## Problema

Se desarrollaron dos versiones de un programa en lenguaje C que simulan la atención de mesas en un restaurante, usando como contexto la formación de salas a partir de mesas atendidas por meseros. Cada sala se forma cada 15 mesas atendidas. Ambas versiones trabajan con el mismo número total de mesas (`30000`) y meseros (`4`), divididas equitativamente.

- **Versión con procesos (`procesos.c`)**: utiliza `fork()` para crear procesos. Se usa memoria compartida con `mmap` y sincronización con `__sync_fetch_and_add`.
- **Versión con hilos (`hilos.c`)**: utiliza `pthread_create()` y sincronización con `pthread_mutex`.

---

## Procedimiento para la Ejecución

### 1. Compilación

Desde una terminal en Linux, ejecutar:

```bash
gcc -o procesos procesos.c -lrt
gcc -o hilos hilos.c -lpthread -lrt
```

### 2. Ejecución

```bash
./procesos
./hilos
```

Ambos programas muestran información detallada sobre el proceso/hilo, rango de mesas atendidas, tiempos de inicio y fin, y el total de salas armadas.

---

## Resultados

Ejemplo de salida:

- Tiempo de inicio y fin de cada hilo o proceso.
- Duración total de la ejecución.
- Número de salas armadas y mesas atendidas.
- Asignación y desempeño por cada mesero.

La diferencia más notable es que la versión con hilos suele mostrar una ejecución más rápida debido a menor sobrecarga que la creación de procesos.

---

## Conclusiones

- **Tiempos de ejecución**: la versión con hilos fue más eficiente en la mayoría de las pruebas.
- **Procesos vs Hilos**:
  - Los procesos son más pesados debido a la duplicación de contexto.
  - Los hilos comparten memoria, lo que facilita la comunicación y mejora el rendimiento.
- **Sincronización**:
  - En procesos se utilizó `__sync_fetch_and_add()` para modificar variables compartidas.
  - En hilos se protegió el acceso a recursos con `pthread_mutex`.

---

## Créditos

Escuela Politécnica Nacional  
Sistemas Operativos  
PhD. Silvia Diana Martínez Mosquera  

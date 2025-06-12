# GRUPOA-proyectoSO: Procesamiento de Transacciones Simuladas
<div align="center">
  <img src="https://img.shields.io/badge/Carrera-Ingeniería%20en%20Ciencias%20de%20la%20Computación-blue?style=for-the-badge" alt="Carrera"/>
  <img src="https://img.shields.io/badge/Grupo-Sistemas%20Operativos-green?style=for-the-badge" alt="Grupo"/>
</div>

---
### **Integrantes:**
* Leandro Alava
* Cristina Moreno
* Erick Topanta
* Jarvin Rios
---

## 📝 Problema

Desarrollar dos versiones de un programa en C que procese 
un conjunto de transacciones simuladas (pueden representar 
consultas a una base de datos, procesamiento de archivos, 
cálculos, etc.). 
Ambas versiones deben atender los mismos grupos de 
transacciones (por ejemplo: GRUPO A hasta GRUPO H).

### Versión 1: Procesos (`fork()`)

* Utiliza la función `fork()` para crear **procesos padre e hijo**.
* Cada proceso se encarga de un **subconjunto de transacciones**.
* Implementa mecanismos de **sincronización** para variables o recursos compartidos (ej., archivos compartidos, pipes).

### Versión 2: Hilos (`pthread.h`)

* Utiliza la biblioteca **`pthread.h`**.
* Cada hilo atiende un **subconjunto de transacciones**.
* Emplea **mecanismos de sincronización** como **mutexes** para proteger los recursos compartidos.

---

## 🍽️ Procedimiento para la Ejecución: Simulación de Restaurante

Para ejemplificar la funcionalidad del programa, simulamos el trabajo de **meseros armando salas en un restaurante**. Cada mesero es responsable de construir salas compuestas por 15 mesas, y la carga total de trabajo se distribuye entre ellos.

1.  **Versión con `fork()` y memoria compartida (`mmap`)**.
2.  **Versión con `pthread` y `mutex` para sincronización**.

---

##  Requisitos para Ejecutar el Programa

Asegúrate de cumplir con los siguientes requisitos:

* **Sistema operativo Linux**.
* **Compilador `gcc`**.

---

## 📂 Archivos del Proyecto

Encontrarás los siguientes archivos en el repositorio:

* `versionmonohilo2.c`: Contiene la implementación de la **versión que usa procesos**.
* `versionmultihilo.c`: Contiene la implementación de la **versión que usa hilos**.

---

## ⚙️ Compilación y Ejecución

Para compilar y ejecutar el programa, sigue las instrucciones según la versión que desees probar:

### Versión con `fork()`

```bash
gcc versionmonohilo2.c -o versionmonohilo2
./versionmonohilo2
```


### Versión con pthread:
```bash
gcc versionmultihilo.c -o versionmultihilo -lpthread
./versionmultihilo
```

## 📊 Resultado Esperado
Ambas versiones del programa, al finalizar su ejecución, mostrarán una tabla detallada con la siguiente información para cada mesero (proceso o hilo):

| Información | Detalle |
| :--- | :--- |
| **Nombre del mesero** | Identificador del mesero |
| **Identificación**| ID del proceso o hilo |
| **Mesas atendidas** | Rango de mesas asignadas |
| **Tiempos relativos**| Hora de inicio y fin |
| **Salas armadas**| Número total de salas completadas |

---

## 🎯 Conclusiones

### Comparación de Tiempos de Ejecución
Ambos programas cumplen la misma tarea: dividir un total de 30,000 mesas entre 4 meseros, lo que equivale a armar 2000 salas (30,000 mesas / 15 mesas por sala). Se observó que, en general, los **hilos (`pthread`)** resultaron ser más rápidos que los **procesos (`fork`)**, especialmente en tareas que implican memoria compartida y requieren menos aislamiento entre las unidades de ejecución.

### Diferencias Encontradas
La siguiente tabla resume las principales diferencias entre `fork()` (procesos) y `pthread` (hilos):

| Característica | `fork()` (Procesos) | `pthread` (Hilos) |
| :--- | :--- | :--- |
| **Memoria** | Cada proceso tiene su propio espacio. | Comparte espacio de memoria. |
| **Comunicación** | Requiere `mmap`, tuberías, etc. | Acceso directo a variables globales. |
| **Sincronización** | Más costosa (uso de operaciones atómicas). | Más sencilla (uso de mutex). |
| **Tiempo de Creación**| Lento (debido al copiado de contexto). | Rápido (hilo dentro del mismo proceso). |
| **Eficiencia** | Menor. | Mayor. |
| **Seguridad/Aislamiento**| Alta (cada proceso es independiente).| Menor (comparten espacio, riesgo de errores).|

---

### ¿En qué casos conviene usar Procesos y en cuáles Hilos?
La elección entre procesos e hilos depende de los requisitos específicos del programa que se vaya a implementar

**Usar procesos (`fork`) cuando:**
* Se necesita **aislamiento completo** entre tareas.
* Las tareas son completamente **independientes** y no comparten memoria.

**Usar hilos (`pthread`) cuando:**
* Las tareas necesitan **memoria compartida** para una comunicación eficiente.
* Se busca un **mejor rendimiento** y una alta velocidad de ejecución con recursos limitados.

---

### Problemas de Sincronización Encontrados y Cómo se Resolvieron
Uno de los problemas que se encontraron en las diferentes procesos fueron:

⚠️ **En la versión con procesos (`fork`)**
* **Problema:** Al ser procesos separados, cada uno opera en su propio espacio de memoria. Para obtener un conteo global de las salas armadas, fue necesario implementar memoria compartida utilizando `mmap`.
* **Solución:** Se empleó la operación atómica `__sync_fetch_and_add`. Esto garantizó que la variable `salasArmadas` se incrementara correctamente sin que se produjeran condiciones de carrera, asegurando la consistencia de los datos entre los procesos.

⚠️ **En la versión con hilos (`pthread`)**
* **Problema:** Todos los hilos comparten la misma variable global `salasArmadasGlobal`. Si múltiples hilos intentan modificarla simultáneamente, pueden surgir condiciones de carrera que lleven a inconsistencias en los datos.
* **Solución:** Se utilizó un `pthread_mutex_t` para proteger la sección crítica donde se actualiza la variable global. El mutex asegura que solo un hilo a la vez pueda modificar `salasArmadasGlobal`, previniendo así las inconsistencias y garantizando la integridad de los datos.

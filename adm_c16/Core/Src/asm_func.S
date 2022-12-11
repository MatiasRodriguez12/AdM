.syntax unified
.thumb
.text

@ Simbolos exportados
.global asm_svc
.global asm_sum
.global asm_zeros
.global asm_productoEscalar32
.global asm_productoEscalar16
.global asm_productoEscalar12
.global asm_pack32to16
.global asm_max
.global asm_filtroVentana10
.global asm_downsampleM
.global asm_invertir
.global asm_audioEco

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_svc (void)
@
.thumb_func
    asm_svc:
        svc 0
        bx lr


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   uint32_t asmSum (uint32_t firstOperand, uint32_t secondOperand)
@
@ Parametros recibidos:
@   r0: firstOperand
@   r1: secondOperand
@
@ Valor de retorno:
@   r0: resultado de la suma de firstOperand y secondOperand
@
.thumb_func
    asm_sum:
        add r0, r1  @ r0 = r0 + r1
        bx lr       @ vuelve adonde fue llamada (especificamente, si "pc"
                    @ es la direccion, en el codigo del programa, de la
                    @ instruccion "bl asm_sum" usada para saltar a
                    @ esta funcion, entonces "lr" contiene ese "pc" + 4).
                    @ el valor de retorno debe quedar en el registro r0.
                    @ En este caso, r0 es el resultado de r0 + r1.

        @ Otras alternativas para el retorno
        @ mov pc,lr
        @ bx lr
        @ push {lr} ... pop {pc}


 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_zeros (uint32_t * vector, uint32_t longitud)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector
@   r1: longitud
.thumb_func               @ Se avisa al emsablador que esta es una funcion
    asm_zeros:            @ Etiqueta/nombre de la función.
        mov r2, 0         @ r2 = 0
        asm_zeros_bucle:  @ Etiqueta de comienzo de bucle.
    str r2, [r0]          @ Asigna 0 a la palabra en dirección r0.
    add r0, 4             @ Pasa a la siguiente palabra/elemento en vector.
    subs r1, 1            @ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    bne asm_zeros_bucle   @ La instrucción se ejecuta mientras el resultado de r1 - 1 no sea cero (Z == 0).
    bx lr                 @ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.


  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_productoEscalar32 (uint32_t * vectorIn, uint32_t * vectorOut, uint32_t longitud, uint32_t escalar)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de salida
@   r2: longitud
@   r3: escalar
.thumb_func               					@ Se avisa al emsablador que esta es una funcion
    asm_productoEscalar32:           		@ Etiqueta/nombre de la función.
    push {r4-r5}							@ Almaceno en stack los valores previos de r4 y r5.
        asm_productoEscalar32_bucle:  		@ Etiqueta de comienzo de bucle.
        ldr r4,[r0]							@ Cargo en r4 el valor de variable a la que esta apuntando r0
        mul r5,r4,r3						@ Cargo en r5 la multiplicacion entre r4 y r3
        str r5,[r1]							@ Almaceno r5 en la direccion que esta apuntando r1
        add r0, 4             				@ Pasa a la siguiente palabra/elemento en vector.
        add r1, 4             				@ Pasa a la siguiente palabra/elemento en vector.
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_productoEscalar32_bucle   	@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    pop {r4-r5}								@ Recupero del stack los valores previos de r4 y r5.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.


  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_productoEscalar16 (uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitud, uint16_t escalar)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de salida
@   r2: longitud
@   r3: escalar
.thumb_func               					@ Se avisa al emsablador que esta es una funcion
    asm_productoEscalar16:            		@ Etiqueta/nombre de la función.
    push {r4-r5}							@ Almaceno en stack los valores previos de r4 y r5.
        asm_productoEscalar16_bucle:  		@ Etiqueta de comienzo de bucle.
        ldrh r4,[r0]						@ Cargo en r4 el valor de variable a la que esta apuntando r0
        mul r5,r4,r3						@ Cargo en r5 la multiplicacion entre r4 y r3
        strh r5,[r1]						@ Almaceno r5 en la direccion que esta apuntando r1
        add r0, 2            				@ Pasa a la siguiente palabra/elemento en vector.
        add r1, 2             				@ Pasa a la siguiente palabra/elemento en vector.
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_productoEscalar16_bucle   	@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    pop {r4-r5}								@ Recupero del stack los valores previos de r4 y r5.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_productoEscalar12 (uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitud, uint16_t escalar)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de salida
@   r2: longitud
@   r3: escalar
.thumb_func               					@ Se avisa al emsablador que esta es una funcion
    asm_productoEscalar12:            		@ Etiqueta/nombre de la función.
    push {r4}								@ Almaceno en stack el valor previo de r4.
        asm_productoEscalar12_bucle:  		@ Etiqueta de comienzo de bucle.
        ldrh r4,[r0],#2						@ Cargo en r4 el valor de variable a la que esta apuntando r0 y apunto al elemeno siguiente (r0 + 2 bytes)
        mul r4,r3							@ Cargo en r4 la multiplicacion entre r4 y r3
        usat r4,12,r4						@ Saturo r4 a 12 bits (unsigned)
        strh r4,[r1],#2						@ Almaceno r4 en la direccion que esta apuntando r1 y apunto al elemeno siguiente (r1 + 2 bytes)
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_productoEscalar12_bucle   	@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    pop {r4}								@ Recupero del stack los valores previos de r4.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_pack32to16 (int32_t * vectorIn, int16_t * vectorOut, uint32_t longitud)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de salida
@   r2: longitud
.thumb_func               					@ Se avisa al emsablador que esta es una funcion
    asm_pack32to16:            				@ Etiqueta/nombre de la función.
        asm_pack32to16_bucle:  				@ Etiqueta de comienzo de bucle.
        ldr r3,[r0],#4						@ Cargo en r3 el valor de variable a la que esta apuntando r0 y apunto al elemeno siguiente (r0 + 4 bytes)
        lsr r3,16							@ Desplazo el contenido de r3 16 lugares a la derecha.
        strh r3,[r1],#2						@ Almaceno r3 en la direccion a la que apunta r1 y paso al siguiente elemento.
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_pack32to16_bucle		   	@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   int32_t max (int32_t * vectorIn, uint32_t longitud)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: longitud

.thumb_func               					@ Se avisa al emsablador que esta es una funcion
    asm_max:        			    		@ Etiqueta/nombre de la función.
    push {r4-r6}							@ Almaceno en stack el valor previo de r4 a r6.
    ldr r2,[r0]								@ Almaceno un valor inicial en r2
    ldr r3,=0								@ Almaceno un indice inicial en r3
    mov r4,r1								@ Almaceno longitud del vector
    mov r5,r1								@ Variable que me indica el indice actual del vector durante el bucle
        asm_max_bucle:  					@ Etiqueta de comienzo de bucle.
        	sub r5,r4,r1					@ Almaceno en r5 la diferencia entre r4 y r1 (obtengo indice actual)
       		ldr r6,[r0],#4					@ Cargo en r6 el elemento actual del vector de entrada
        	cmp r6,r2						@ Comparo r6 con r2
        	itt gt							@ Ejecucion condicional si r6>r2
        	movgt r3,r5						@ si r6>r2, actualizo el indice donde se encuentra el elemento mayor temporal
        	movgt r2,r6						@ si r6>r2, actualizo el valor del elemento mayor temporal
        subs r1, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_max_bucle				   	@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    mov r0,r3								@ Para poder devolver el indice, cargo r3 en r0.
    pop {r4-r6}								@ Recupero del stack los valores previos de r4 a r6.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_filtroVentana10(uint16_t * vectorIn, uint16_t * vectorOut, uint32_t longitudVectorIn)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de salida
@   r2: longitud
@   r3: ind_max
@   r4: ind_min, indice para bucle 2
@   r5: suma
@   r6: cantidad
@   r7: indice actual para bucle
@	r8: longitud (fijo)
@	r9: indice para segundo bucle
@	r10: direccion del primer elemento para bucle suma
@	r11: indice auxiliar para obtener direccion de primer elemento de bucle suma
@	r12: carga temporal de los valores para sumar

.thumb_func               					@ Se avisa al emsablador que esta es una funcion

    asm_filtroVentana10:            		@ Etiqueta/nombre de la función.
    	push {r4-r12}						@ Almaceno en stack el valor previo de r4 a r12
		mov r8,r2							@ Almaceno longitud de manera constante
		mov r9,r0							@ Almaceno direccion del elemento 0 de manera constante

		asm_filtroVentana10_bucle:
		sub r7,r8,r2						@ Obtengo indice actual del bucle

		add r3,r7,5							@ Obtengo indice maximo
		cmp r3,r8							@ Comparo indice maximo con longitud
		itt ge
		movge r3,r8							@ Si indice maximo es mayor a la longitud, cargo longitud-1 como indice maximo
		subge r3,1

		mov r11,2							@ Inicializo r11 con un valor de 2
		mov r10,r9							@ Inicializo r10 con la direccion del elemento 0 del vector de entrada
		sub r4,r7,5 						@ Obtengo indice minimo
		cmp r4,0
		itee le
		movle r4,0							@ Si indice minimo me da negativo, lo igualo a 0 (direccion inicial es igual a la del elemento 0)
		mulgt r11,r4,r11					@ Si es mayor a 0, multiplico el valor del indice minimo por 2
		addgt r10,r11						@ Sumo el resultado anterior a r10. Obtengo direccion donde deberia iniciar a recorrer el bucle

			mov r5,0						@ Inicializo r5 a 0
			mov r6,0						@ Inicializo r6 a 0
			suma_bucle:						@ Inicia bucle para calcular la suma de los elemenos de la ventana
			ldrh r12,[r10],#2				@ Cargo en r12 el valor del indice actual del bucle
			add r5,r12						@ R5 se encarga de ir acumulando la suma de los elementos de la ventana
			add r6,1						@ R6 se encarga de ir contando cuantos elementos tiene la ventana
			add r4,1						@ Incremento r4 en una unidad
			cmp r4,r3						@ Si r4 es igual a R5, quiere decir que ya se recorrieron todos los elementos de la ventana
			ble suma_bucle
		udiv r5,r6							@ Divido la suma acumulada con la cantidad de elementos de la ventana
		strh r5,[r1],#2						@ Almaceno el promedio en el vector de salida e incremento en 2 la direccion de salida
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_filtroVentana10_bucle		@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
	pop {r4-r12}							@ Recupero del stack los valores previos de r4 a r12.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_downsampleM (int32_t * vectorIn, int32_t * vectorOut, uint32_t longitud, uint32_t N);
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de salida
@   r2: longitud
@   r3: indicacion de cada cuantas muestras se debe eliminar
@	r4: indice para controlar cantidad de elementos recorridos hasta el momento
@	r5: Valor que contiene la dirección a la que apunta r0
@	r6: registro que contiene un valor de 0
.thumb_func               					@ Se avisa al emsablador que esta es una funcion
    asm_downsampleM:            			@ Etiqueta/nombre de la función.
    push {r4-r6}							@ Almaceno en stack los valores previos de r4-r6.
    mov r4,#1								@ Inicializo el indice en 1
    mov r6,0								@ Cargo en r6 un valor igual a 0
        asm_downsampleM_bucle:  			@ Etiqueta de comienzo de bucle.
        ldr r5,[r0],#4						@ Cargo en r5 el contenido de la dirección a la que apunta r1.
        cmp r4,r3							@ Comparo indice r4 con r3.
        ittee lt							@ Defino condicional
        strlt r5,[r1],#4					@ Si r4<r3, cargo el valor del elemento del vector de entrada en su correspondiente elemento del vector de salida.
        addlt r4,#1							@ Si r4<r3, incremento en 1 el indice r4
        strge r6,[r1],#4					@ Si r4>=r3, cargo 0 en el elemento que corresponde al vector de salida
        movge r4,#1							@ Si r4>=r3, inicializo el indice en 1 nuevamente
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_downsampleM_bucle   	@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    pop {r4-r6}								@ Recupero del stack los valores previos de r4.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_invertir (uint16_t * vector, uint32_t longitud);
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: longitud
@	r2: indice para conocer la mitad del vector
@	r3: contenido del elemento i del vector.
@	r4:	indice auxiliar para determinar la direccion correspondiente al elemento i a invertir
@	r5:	direccion correspondiente al elemento i a invertir
@	r6: contenido del elemento a invertir con el elemento i
@	r7: contiene la direccion r0 de manera constante
.thumb_func               				@ Se avisa al emsablador que esta es una funcion
    asm_invertir:            			@ Etiqueta/nombre de la función.
    push {r4-r7}						@ Almaceno en stack los valores previos de r4-r7.
    mov r2,#2							@ Inicializo r2 con un valor igual a 2
    udiv r2,r1,r2						@ Divido la longitud entre 2 y lo almaceno en r2 (indice de mitad de vector)
    sub	r1,#1							@ Resto 1 a la longitud, obtengo indice maximo del vector
    mov r5,r0							@ Inicializo r5 con un valor igual a r0
    mov r7,r0							@ Inicializo r5 con un valor igual a r0. r5 se mantendra constante
        asm_invertir_bucle:  				@ Etiqueta de comienzo de bucle.
        ldrh r3,[r0]						@ Cargo en r5 el contenido de la dirección a la que apunta r1.
        mov	r4,2							@ Inicializo r4 con un valor igual a 2
 		mul	r4,r1,r4						@ Multiplico r1 por 2 y almaceno en r4. Obtengo cuantos elementos debo sumar a r0 para obtener la direccion de su elemento correspondiente a invertir
		sub r1,1							@ Resto 1 a r1.
		add	r5,r4,r7						@ Obtengo la direccion del elemento a invertir el elemento i
		ldrh r6,[r5]						@ Obtengo el contenido del elemento a invertir con el elemento i
		strh r3,[r5]						@ Cargo en el elemento i el contenido correspondiente
		strh r6,[r0],#2						@ Mandl el contenido i a su nuevo lugar
        subs r2, 1           				@ Resta 1 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    	bne asm_invertir_bucle   			@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    pop {r4-r7}								@ Recupero del stack los valores previos de r4-r7.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Prototipo en "C":
@   void asm_audioEco (int16_t * vectorIn,int16_t * vectorOut,uint32_t longitud)
@
@ Parametros recibidos:
@   r0: dirección de memoria de inicio del vector de entrada
@	r1: dirección de memoria de inicio del vector de entrada
@	r2: longitud
@	r3: Indice para primer bucle (dejar pasar valores sin eco)
@	r4: Contendio de la direccion a la que apunta r0.
@	r5: Direccion donde se debe obtener el eco.
@	r6: Contenido de r5 (eco a agregar).
@	r7: Registro con contenido 0 (usado para producir division por 2 en SIMD).

.thumb_func               				@ Se avisa al emsablador que esta es una funcion
    asm_audioEco:            			@ Etiqueta/nombre de la función.
    push {r4-r7}						@ Almaceno en stack los valores previos de r4-r7.
    mov r3,#442							@ Inicializo r3 con un valor igual a 442 (882/2+1).
  	mov r7,0							@ Inicializo r7 con 0 (usado para instruccion SIMD shadd16).
	mov r5,r0							@ Inicializo r5 con un valor igual a r0.
	add r2,2							@ Adiciono 2 unidades a r2 (para recorrer el vector completamente)
        asm_audioEco_delay_bucle:  				@ Etiqueta de comienzo de bucle.
        sub r3,1						@ Resto 1 unidad a r3 por cada iteracion de bucle
        sub r2,2						@ Resto 2 unidades a r2 por cada iteracion de bucle
        ldr r4,[r0],#4					@ Cargo en r4 el contenido al que apunta r0, y sumo 4 bytes.
        cmp r3,0						@ Comparo r3 con 0. Se genera condicional itt
        itt gt
        strgt r4,[r1],#4				@ Si r3>0, almaceno el contenido de r4 en la direccion a la que apunta r1, y sumo 4 bytes
        bgt asm_audioEco_delay_bucle	@ Si r3>0, retorno al bucle asm_audioEco_delay_bucle
	sub r0,4								@ Resto 4 bytes a r0 para no perder elementos
    asm_bucle_eco:							@ Etiqueta de comienzo de bucle.
    ldr r4,[r0],#4						@ Cargo en r4 el contenido al que apunta r0, y sumo 4 bytes.
    ldr r6,[r5],#4						@ Cargo en r6 el contenido al que apunta r1 (eco), y sumo 4 bytes.
    shadd16	r6,r6,r7					@ Sumo r6 un valor a 0 y divido por 2 (SIMD)
    sadd16	r4,r4,r6					@ Sumo registros r4 y r6 (SIMD) y almaceno en r4
    str r4,[r1],#4						@ Almaceno r4 en la direccion a la que apunta r1, y sumo 4 bytes

    subs r2, 2           				@ Resta 2 a la cantidad de elementos y actualiza flags de estado del procesador. En particular, estamos interesados en el flag Z.
    bne asm_bucle_eco   			@ La instrucción se ejecuta mientras el resultado de r2 - 1 no sea cero (Z == 0).
    pop {r4-r7}								@ Recupero del stack los valores previos de r4-r7.
    bx lr                 					@ La instrucción de salto no se ejecutó porque Z == 1, volvemos a quién nos llamó.

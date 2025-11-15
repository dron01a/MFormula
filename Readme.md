# MFormula 

## Unusual "calculator"

Скриптовый язык программирования для решения математических и других задач. Написан на C/C++ с использованием стандартной библиотеки.
Изначально он создавался как сложный калькулятор математических выражений, позже были добавлены "if", "массивы" и "другие радости" жизни.
По мере необходимости будут вноситься дополнительные изменения.

A scripting language for solving mathematical and other tasks. Written in C/C++ using the standard library.
It was originally created as a parser of mathematical expressions, later "if", "array's" and "other joys" of life were added.
Further modifications will be introduced as needed. Ideally, get a program for calculating automatic control systems.

<img src="doc/nauka.jpg" 
     alt="Слава советским трудящимся и деятелям науки!"
     aligin="left">

## Current features
__defined variables__<br>

    var a = 1917;

__defined lists__<br>

    var list = { 1, 1, 1 };

__defined functions__<br>

    func def(var a, b){
        return (a+b)*10-(a+b);
    }

__if-else block__<br>

    if(a > b){
        print("a > b");
    }

__for loop__<br>

    for(var i = 0; i != 3 ; i++ ){
        print(i);
    }

__while loop__<br>

    while(true){
        // code
    }

__include files__<br>

    // lib -> name of file with code
    include lib; 

## Example of code

    #./examples/fibo.math

    var num1 = 0;   // first num 
    var num2 = 1;   // second num
    var temp;       // temp var
    var next;       // next num
    var n = 188;    // position

    if (n >= 1){
        print("1 --> 0");
    }
    if (n >= 2){
        print("2 --> 1");
    }
    for(var _count = 0; _count < n - 2; _count++ ){
        next = num1 + num2;
        temp = _count+3;
        print("%temp% --> %next%");
        num1 = num2;
        num2 = next;
    }


## Warning

Операции * и % работают только для обычных чисел, для комплексных чисел требуется отдельная реализауция
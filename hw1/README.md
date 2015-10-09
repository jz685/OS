command convert:
used to convert between styles
http://blog.csdn.net/huntinux/article/details/8531178
http://blog.csdn.net/fdipzone/article/details/39651709

command display:
used to display a photo

command find
http://blog.csdn.net/wenchao126/article/details/8071526
find . -name "*.jpg" -exec display {} \;


4)将find出来的东西拷到另一个地方  copy files that FIND command find to some place else.

find *.c -exec cp '{}' /tmp ';'  why?   what does '{}' /tmp ';'  mean here!

			共享内存
描述：
	共享内存是被多个进程共享的一部分物理内存，也是一种最快的方法。

1.使用步骤：
	1>.创建共享内存，使用int shmget(key_t key,int size,int shmflg)函数。
	key:标识内存key,如是IPC_PRIVATE，将创建一块新的共享内存。若key为0，shmflg为IPC_PRIVATE
	同样创建一块新的共享内存。
	如果失败返回-1，否则（成功）返回共享内存一个标识符
	2>.映射共享内存，将这段创建的内存映射到具体的进程中去，使用shmat函数。
	使用int shmat(int shmid,char * shmaddr,int flag)
	shmid：shmget返回的id编号
	shmaddr:用这个参数获取值地址。如果是0表示要系统自动的分配一个地址
	flag:决定以什么方式来确定来映射地址(通常为0)
	如果失败返回-1，否则（成功）返回共享内存一个标识符
	3>.解除映射int shmdt(char * shmaddr):
	shmaddr:获取的值地址
	当一个进程不需要共享内存时需要从进程地址空间中脱离出来。
主要内容：
	1.创建共享内存，在父进程中把参数传到共享内存中在子进程中读取。


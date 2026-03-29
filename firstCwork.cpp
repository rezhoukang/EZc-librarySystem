#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h> //屎山风格
#include <stdlib.h>//入库代表创文件
#include <string.h>
void start();
char welcome();
void all_core(char role);
/*检查账密(pure)*/int check(const char* user_name, const char* user_password)//const的目的是提醒变量只读不写
{
	FILE* file = fopen("password.txt", "r"); // 打开文件以只读模式
	if (file == NULL) {
		perror("此账号未注册或正在注册\n");
		return 0;
	}
	char file_user_name[50], file_password[50];
	while (fscanf(file, "%s %s", file_user_name, file_password) == 2) {//临时抽取2段%s作为检查,每次file会指向下一个
		file_user_name[strcspn(file_user_name, "\n")] = '\0';
		file_password[strcspn(file_password, "\n")] = '\0';
		if (strcmp(file_user_name, user_name) == 0 && strcmp(file_password, user_password) == 0) {
			fclose(file);
			//temp_name(user_name);为什么加了会错
			return 1; // 匹配成功，登录成功，注册失败
		}
		else if (strcmp(file_user_name, user_name) == 0) {
			fclose(file);
			return 2; // 密码对，用户名相同注册失败
		}
	}
	fclose(file);
	return 0; // 匹配失败,登录失败,注册成功
}
/*注册账密入库(pure)*/int user_in(char* user_name, char* password)
{
	FILE* file;
	file = fopen("password.txt", "a"); // 追加模式打开文件
	if (file == NULL) {
		perror("error");
		return 1; // 返回错误状态
	}
	fprintf(file, "%s %s\n", user_name, password);
	fclose(file); // 关闭文件
	printf("用户名和密码已成功写入到 password.txt 文件！\n");
	return 0;
}
/*图书基本结构(pure)*/typedef struct book// 定义图书结构体//用struct不用typedef，香味少一半呀,新类型和老类型都可以用哦，我都写一样就不会错了哈哈
{
	int id;             // 图书ID
	char title[100];    // 图书标题
	char author[100];   // 作者
	int year;           // 出版年份
	struct book* next;  // 指向下一个图书的指针
} book;
/*记录当前用户名(pure)*/void temp_name(char* name)
{
	FILE* file;
	file = fopen("temp.txt", "w");
	fprintf(file, "%s", name);
	fclose(file);
}
/*数据进链表(pure)*/void data_intolink(book* temp)
{
	printf("请输入图书ID(按0退出): ");
	scanf("%d", &temp->id);
	getchar();  // 吸收多余的换行符
	if (temp->id == 0)return;
	printf("请输入图书标题: ");
	fgets(temp->title, sizeof(temp->title), stdin);
	temp->title[strcspn(temp->title, "\n")] = '\0';
	printf("请输入作者: ");
	fgets(temp->author, sizeof(temp->author), stdin);
	temp->author[strcspn(temp->author, "\n")] = '\0';
	printf("请输入出版年份: ");
	scanf("%d", &temp->year);
	getchar();
}
/*创建链表*/book* creatlink()//返回结构体指针类型的函数
{
	book* head, * p, * q;
	head = (book*/*这个是为了返回指针赋值给head所以加了个星号*/)malloc(sizeof(book));//建了一个区块
	head->next = NULL;
	p = head;//指向head
	q = (book*)malloc(sizeof(book));//q又建了一个区块
	do {//从q区块开始存储，head空着
		data_intolink(q);
		if (q->id == 0) break;
		p->next = q;//q跟着此时的p->next
		p = q;//p同q此时地址，pq要逃跑
		q = (book*)malloc(sizeof(book));//q跑走了，去建了一个新区块
	} while (1);
	p->next = NULL;
	return head;
}
/*打印链表       我最后貌似没怎么用，蛮放在这里吧        */void printlink(book* head)
{
	book* p;
	printf("%-10s%-30s%-30s%-10s\n", "id", "title", "author", "year");
	for (p = head->next; p != NULL; p = p->next) {
		printf("%-10d%-30s%-30s%-10d\n", p->id, p->title, p->author, p->year);
	}
}
/*删除admin库的书*/void book_out(int out_id)
{
	FILE* file, * temp, * name;
	int id;
	char title[50];
	char author[50];
	char role[50];
	int year, correct = 0;
	file = fopen("admin", "r");//同下
	if (file == NULL) {
		printf("图书馆没书啦\n");
		return;
	}
	temp = fopen("new_admin", "w");
	while (fscanf(file, "%d\t%s\t%s\t%d", &id, title, author, &year) == 4) {
		if (id == out_id)correct++;
	}
	if (correct == 0) {
		fclose(file);fclose(temp);
		remove("new_admin");
		printf("没有这本书\n");
		return;
	}//避免凭空借书
	rewind(file);//从头开始
	while (fscanf(file, "%d\t%s\t%s\t%d", &id, title, author, &year) == 4) {
		if (id != out_id) {
			fprintf(temp, "%d\t%s\t%s\t%d\n", id, title, author, year);//拷贝
		}
	}
	fclose(file);fclose(temp);
	if (remove("admin") != 0) {
		printf("删除文件失败！\n");
		return;
	}

	if (rename("new_admin", "admin") != 0) {
		printf("重命名文件失败！\n");//好奇怪，我加了检测点，再测又正常了
		return;
	}
	
	printf("图书删除成功！\n");
}
/*删除当前用户库的书,同时入admin库*/void check_outin(int out_id)
{
	FILE* now_user, * temp, * temp_user, * admin;
	int id;
	char title[100];
	char author[100];
	int year, correct = 0;
	char user[50];//当前用户名
	temp_user = fopen("temp.txt", "r");
	fgets(user, sizeof(user), temp_user);//第二个是最大输入流量，不必严丝合缝
	fclose(temp_user);
	user[strcspn(user, "\n")] = '\0';
	now_user = fopen(user, "r");//同上
	if (now_user == NULL) {
		system("cls");
		perror("你没借过书，还什么书？\n");
		all_core('1');
	}
	temp = fopen("new.txt", "w");
	admin = fopen("admin", "a");
	while (fscanf(now_user, "%d\t%s\t%s\t%d", &id, title, author, &year) == 4) {//我又可以三角法则，又可以卫语句，实力不用多言
		if (id == out_id)
			correct++;
	}
	if (correct == 0) {
		fclose(now_user); fclose(temp); fclose(admin);
		system("cls");
		printf("没有这本书，别乱还\n");
		all_core('1');}//避免凭空借书
	rewind(now_user);
	while (fscanf(now_user, "%d\t%s\t%s\t%d", &id, title, author, &year) == 4) {
		if (id != out_id) {
			fprintf(temp, "%d\t%s\t%s\t%d\n", id, title, author, year);//拷贝
		}
		else if (id == out_id) {
			fprintf(admin, "%d\t%s\t%s\t%d\n", id, title, author, year);//从admin添加
		}
	}
	fclose(now_user); fclose(temp); fclose(admin);//不关闭不保存
	remove(user);//删除
	rename("new.txt", user);
	printf("还书成功！\n");
}
/*查看admin库的书*/void print_admin()
{
	system("cls");
	FILE* file = fopen("admin", "r"); // 打开文件以只读模式
	if (file == NULL) {
		system("cls");
		perror("图书馆没书辣");
		all_core('1');
	}
	int id;
	char title[100];
	char author[100];
	int year;
	printf("书库：\n");
	printf("%-10s%-30s%-30s%-10s\n", "id", "title", "author", "year");
	while (fscanf(file, "%d\t%s\t%s\t%d", &id, title, author, &year) == 4) {//临时抽取4个作为检查,每次file会指向下一个
		title[strcspn(title, "\n")] = '\0';
		author[strcspn(author, "\n")] = '\0';
		printf("%-10d%-30s%-30s%-10d\n", id, title, author, year);
	}
	fclose(file);//这个忘记关闭了，我bug找半死
}
/*查看当前用户借的书*/void print_user()
{
	system("cls");
	char name[50];
	FILE* user = fopen("temp.txt", "r");
	fgets(name, sizeof(name), user);
	name[strcspn(name, "\n")] = '\0';
	FILE* file = fopen(name, "r"); // 打开文件以只读模式
	if (file == NULL) {
		system("cls");
		perror("你并没有借过书");
		all_core('1');
	}
	int id;
	char title[100];
	char author[100];
	int year;
	printf("你借的书：\n");
	printf("%-10s%-30s%-30s%-10s\n", "id", "title", "author", "year");
	while (fscanf(file, "%d\t%s\t%s\t%d", &id, title, author, &year) == 4) {//临时抽取4个作为检查,每次file会指向下一个
		//id[strcspn(id, "\n")] = '\0';
		title[strcspn(title, "\n")] = '\0';
		author[strcspn(author, "\n")] = '\0';
		//year[strcspn(year, "\n")] = '\0';
		printf("%-10d%-30s%-30s%-10d\n", id, title, author, year);
	}
	fclose(file);//这个忘记关闭了，我bug找半死
}
/*按名字入库*/void book_in(book* head)//对于admin入库就是进书库了；对于user入库就是被他们借走的记录，避免乱还
{
	FILE* log, * temp_name;
	char name[50];
	int check_id;
	char check_title[50];
	char check_author[50];
	int check_year; int cant_bookin=0;
	temp_name = fopen("temp.txt", "r");
	fgets(name, sizeof(name), temp_name);//第二个是最大输入流量，不必严丝合缝
	name[strcspn(name, "\n")] = '\0';
	fclose(temp_name);
	log = fopen(name, "a");//a和a+都可以从无创造新文件
	book* p;
	FILE* admin = fopen("admin", "r");
	if (strcmp(name, "admin") != 0)//三角法则和卫语句谁更权威hhh
		for (p = head->next; p != NULL; p = p->next) {// 将图书数据逐条写入文件，每本书占一行
			while (fscanf(admin, "%d\t%s\t%s\t%d", &check_id, check_title, check_author, &check_year) == 4) {
				if (p->id == check_id && strcmp(p->title, check_title) == 0 && strcmp(p->author, check_author) == 0 && p->year == check_year) {
					cant_bookin++;
				}
			}
			if (cant_bookin == 0) {
				fclose(admin);
				fclose(log);
				system("cls");
				printf("求你先看看有什么书再借吧\n");
				all_core('1');
			}
			rewind(admin);
			while (fscanf(admin, "%d\t%s\t%s\t%d", &check_id, check_title, check_author, &check_year) == 4){
				if (p->id == check_id && strcmp(p->title, check_title) == 0 && strcmp(p->author, check_author) == 0 && p->year == check_year) {
					fprintf(log, "%d\t%s\t%s\t%d\n", p->id, p->title, p->author, p->year);//加入
					fclose(admin);
					fclose(log);
					book_out(p->id);//若是user加入，则admin删除
				}
			}
		}
	else
		for (p = head->next; p != NULL; p = p->next)
			fprintf(log, "%d\t%s\t%s\t%d\n", p->id, p->title, p->author, p->year);//加入
	fclose(admin);
	fclose(log);
	printf("图书信息已成功保存到 %s 文件！\n", name);
}
/*用户和管理员的二层选择实现*/void core_choose(int role)
{
again_choose:
	book* link = NULL;//像int声明一样
	int out_id;
	char choose = getchar();
	getchar();
	if (role == 1) {//user
		switch (choose) {
		case '0': system("cls"); start(); break;//返回
		case '1': do {
			printf("请输入还的图书ID：(按0退出)");
			scanf("%d", &out_id); getchar();
			if (out_id == 0) break;
			check_outin(out_id);
		} while (1); system("cls"); all_core('1'); break;//还书
		case '2': link = creatlink(); book_in(link); system("cls"); printf("已成功借阅\n"); all_core('1'); break;//借书，避免了凭空借
		case '3': print_admin(); all_core('1'); break;//查看
		case '4': print_user(); all_core('1'); break;//查看
		default:printf("请重新正确输入!\n"); goto again_choose;
		}
	}
	else if (role == 3) {//admin
		switch (choose) {
		case '0': system("cls"); start(); break;//返回
		case '1': /*  book *  */link = creatlink();
			book_in(link); system("cls"); printf("已成功添加如下图书\n"); printlink(link);  all_core('3'); break;//添加书
		case '2': do {
			printf("请输入要删除的图书ID：(按0退出)");
			scanf("%d", &out_id); getchar();
			if (out_id == 0) break;
			book_out(out_id);
		} while (1); system("cls");  all_core('3'); break;//删除书
		case '3':  print_admin(); all_core('3'); break;//查书
		default:printf("请重新正确输入!\n"); goto again_choose;
		}
	}
}
/*用户和管理员的二层页*/void all_core(char role)
{
	if (role == '1') {//user
		//巧用choose他难道真是天才？
		printf("------------------------------user\n");
		printf("\t  1. 还书\n");
		printf("\t  2. 借书 \n");
		printf("\t  3. 查看书库的书\n");
		printf("\t  4. 查看你借的书\n");
		printf("\t  0. 返回上一级\n");
		printf("----------------------------------\n");
		printf("请输入您的需求:");
		core_choose(1);
	}
	else if (role == '3') {//admin
		printf("------------------------------admin\n");
		printf("\t  1. 添加书\n");
		printf("\t  2. 删除书\n");
		printf("\t  3. 查看书库的书\n");
		printf("\t  0. 返回上一级\n");
		printf("----------------------------------\n");
		printf("请输入您的需求:");
		core_choose(3);
	}
}
/*登录界面*/void login()
{
	char user_name[50], user_password[50];
	printf("<*********请输入您的账号和密码********>\n");
	printf("账号:\t");
	fgets(user_name, sizeof(user_name), stdin);//结尾去个\n加0，本来还想保留\n的
	user_name[strcspn(user_name, "\n")] = '\0';
	printf("密码:\t");
	fgets(user_password, sizeof(user_password), stdin);
	user_password[strcspn(user_password, "\n")] = '\0';
	if (check(user_name, user_password) == 1) {
		system("cls");
		printf("登陆成功\n");
		temp_name(user_name);
		return;
	}
	else {
		system("cls");
		printf("damn，bro你的密码或者账号错误了\n");
		//while (1);//没意义的备注很累，比如说这个是为了调试不退出;
		start();
	}
}
/*注册界面*/void signup()
{
	char user_name[50], user_password[50];
	printf("<*********请输入您的账号和密码********>\n");
	printf("账号:\t");
	fgets(user_name, sizeof(user_name), stdin);
	user_name[strcspn(user_name, "\n")] = '\0';
	printf("密码:\t");
	fgets(user_password, sizeof(user_password), stdin);
	user_password[strcspn(user_password, "\n")] = '\0';
	if (check(user_name, user_password) == 0) {
		user_in(user_name, user_password);// 保存用户名和密码
		system("cls");
		printf("注册成功！\n");
	}
	else {
		system("cls");
		printf("用户名已存在,注册失败\n");
		start();
	}
}
/*超级登录*/void admin()
{
	char user_name[50], user_password[50];
	printf("\t账号:");
	fgets(user_name, sizeof(user_name), stdin);
	user_name[strcspn(user_name, "\n")] = '\0';
	printf("\t密码:");
	fgets(user_password, sizeof(user_password), stdin);
	user_password[strcspn(user_password, "\n")] = '\0';
	if (strcmp(user_name, "admin") == 0 && strcmp(user_password, "114514") == 0) {//相同等于0
		system("cls");
		printf("登陆成功\n");
		temp_name(user_name);
	}
	else {
		system("cls");
		printf("damn,");
		printf("bro!  admin密码是\"114514\"\n");
		//while (1);//没意义的备注很累，比如说这个是为了调试不退出;
		start();
	}
}
/*首页选择实现(选择放在对应的上面)*/void welcome_choose(char choose)
{
	switch (choose) {
	case '1': login(); return; break;
	case '2': signup(); start(); break;
	case '3': admin(); break;
	case '0': exit(0); break;
	default:system("cls"); printf("请重新正确输入!\n");  start(); break;
	}
}
/*首页*/char welcome()//欢迎界面
{
	printf("=====欢迎来到至诚书库管理系统=====\n");
	printf("----------------------------------\n");
	printf("\t  1. 用户登录\n");
	printf("\t  2. 注册账号\n");
	printf("\t  3. 管理员登录\n");
	printf("\t  0. 退出\n");
	printf("----------------------------------\n");
	printf("<*********请输入您的需求:********>\n");
	printf("==================================\n");
	char choose = getchar();
	getchar();
	welcome_choose(choose);
	return choose;
}
/*梦开始的地方*/void start()
{
	char choose = welcome();
	//上登入，下内核
	all_core(choose);
	while (1);//没意义的备注很累，比如说这个是为了调试不退出;
	//这个函数是为了返回上一页而作，前面试了嵌套，返回值总是第一个选择，本来想最后一个返回值用文件存，有点麻烦。用全局变量怕乱掉。所以写了这个
	//这段代码算是我在完成后最后新写的了，其他逻辑都写好了，就剩返回上一级没写
}
int main()
{
	start();
	return 0;
}






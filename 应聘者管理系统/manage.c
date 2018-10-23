#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

void insert_student_information();
void search_student_information();

void delete_student();
void print_student_addition_order();
void print_student_number_order();
void save_to_file();
void exit_system();
void menu();
void menu_system();
void menu1();
void menu1_system();
void menu2();
void menu2_system();
void menu3();
void menu3_system();
int put_order();
void cover_to_file();

void admin_system();
void admin_student( );//管理员管理学生账户
void admin_hr();//管理员管理hr账号
void admin_change(); //管理员修改自己账号密码

void help_system();
//void admin_student_menu();
void login_student();
void login_hr();
void login_system();

//创建节点
typedef struct student 
{
	char ID[20];
	char name[20];
	char sex[3];
	char class[10];
	char tel[20];
	char adress[30];
	char grade[10];
	int pFlag;
	struct student *pre,*next;

}*STU,STUDENT;

typedef struct order
{
	char username[100];
	char password[100];
	struct order *pre, *next;
}*ORD,ORDER;

void delete( STU a );
void insert(STU a, STU p, STU n);
void read_file();
void inputone(STU s);

//创建全局变量

STU head=NULL;
STU iend=NULL;
ORD head_student=NULL,head_hr=NULL;
ORD iend_student=NULL,iend_hr=NULL;
	

//FILE *fp;

void inputone(STU s)
{
		printf("\n****************请依次输入应聘者信息********************\n");
		printf("序号：");
		scanf("%s",s->ID);
		printf("姓名：");
		scanf("%s",s->name);
		printf("应聘岗位：");
		scanf("%s",s->class);
		printf("性别：");
		scanf("%s",s->sex);
		printf("电话：");
		scanf("%s",s->tel);
		printf("地址：");
		scanf("%s",s->adress);
		printf("成绩：");
		scanf("%s",s->grade);
}
void read_file()
{
	FILE *fp;
	STU s;
	if((fp=fopen("information.txt","at+"))==NULL)
	{
		printf("读取文件出错");
		exit(0);
	}
	head=(STU)malloc(sizeof(STUDENT));
	head->next=NULL;
	iend=head;
	while(!(feof(fp)))
	{
		s=(STU)malloc(sizeof(STUDENT));
		fscanf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n",s->name,s->ID,s->sex,s->class,s->tel,s->adress,s->grade);
		iend->next=s;
		s->pre=iend;
		iend=s;
	}
	iend->next=NULL;
	fclose(fp);
	menu1();
	return;


}


//添加应聘者信息
void insert_student_information()
{
	int flag = 1;
	STU s,r;	
//	head=(STU)malloc(sizeof(STUDENT)); //最开始的head要记得初始化（已实现在主函数中初始化1）
	int n=0;
	for(r=head->next; r; r=r->next)
	n++;

	if(n==0)	
	iend =head;

	while(flag)
	{
		system("clear");
		s=(STU)malloc(sizeof(STUDENT));
		inputone(s);
		s->next=NULL;
		s->pre=iend;
		iend->next=s; //这里先开始有问题是因为iend指向head而head没有进行初始化
		iend=s;
		printf("继续输入请按1，返回上一级菜单请按2:");
		scanf("%d",&flag);
		if(flag==1)
			continue;
		if(flag==2)
		{
			system("clear");			
			break;	
		}

	}
//	    iend->next=NULL;
		free(s);
		menu1();
		return ;
}

//查询应聘者信息
void search_student( )
{
	
	//system("clear");
	char sname[20];
	int flag=1, flag2=1;

	while(flag2)
	{
		system("clear");
		printf("\n请输入你要查询的应聘者姓名：");
		scanf("%s",sname);
		iend=head->next;
		while(iend)
		{
			if(strcmp(sname,iend->name)==0)
			{
				printf("%s\t%s\t%s\t%s\t%s\t%d\t%s\n",iend->ID,iend->name,iend->sex,iend->class,iend->tel,iend->adress,iend->grade);
				flag = 0;
				//break;
				//continue;
			
			}
			iend=iend->next;
	
		}
		if(flag==1)
		{
			printf("该用户不存在哦~\n");
		
		}
		printf("继续查询请按1，返回上一层请按2:\n");
		scanf("%d",&flag2);
		if(flag2==1)
			continue;
		if(flag2==2)
		{
			system("clear");
			menu1();
			break;
		
		}

	}
	return ;


}
//删除一个节点的函数
/*void delate( STU iend )
{
	iend->next->pre = iend->pre;
	iend->pre->next = iend->next;
//	iend = NULL;
	free(iend);

}*/

//删除学生信息
void delete_student(void)
{
	
	STU pPre, pNext;
	char sname[20];
	int flag=1,flag2=1;

	while(flag2)
	{
		system("clear");
		iend=head;
		printf("请输入你要删除的用户姓名：");
		scanf("%s",sname);
		
		while(iend->next)
		{

			pPre=iend;
			iend=iend->next;
			pNext=iend->next;//放到前边

			if(strcmp(sname,iend->name)==0 )
			{
			
				if(iend->next== NULL)
				{
					system("clear");
					menu1();
					break;
				}
				else
				{
			/*	(p->pre)->next = p->next;
				(p->next)->pre = p->pre;
				free(p);*/
				pNext->pre = pPre;
				pPre->next = pNext;
				free(iend);
				flag=0;
				}
			
			}
		
		}
		cover_to_file();
		if(flag==1)
		{
			printf("要删除的用户不存在哦~\n");
		
		}

		printf("\n继续删除请按1，返回上一层菜单请按2:");
		scanf("%d",&flag2);
		if(flag2==1)
			continue;
		if(flag2==2)
		{

			system("clear");
			menu1();
			break;
			
		}

	}
	return;

}

//将a插入p与n之间
void insert( STU a, STU p, STU n)
{
	a->pre = p;
	p->next = a;
	a->next = n;
	n->pre = a;

}

//按写入顺序输出
void print_student_addition_order(void)
{
	iend=head->next;
	int flag;
	//iend=head;
	while(iend)
	{
	/*	iend=iend->next;
		if(iend==NULL)
		{
			system("clear");
			printf("没有用户了哦\n");
			menu1();
			break;
		
		}
		else*/
		printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",iend->ID,iend->name,iend->sex,iend->class,iend->tel,iend->adress,iend->grade);
		iend=iend->next;
	
	}
	printf("\n返回上一层请按2:");
	scanf("%d",&flag);
	if(flag==2)
	{
		system("clear");
		menu1();
		return;
	}
	else
	{
		printf("请输入有效数字！\n");
	
	}

}
//按学号顺序输出
void print_student_number_order(void)
{
	char min[20]={'9'};
	int flag;
	int n=0,i,j;
	
	STU t;
	iend = head->next;
	t = head->next; 
	while(iend)
	{
		n++;
		iend->pFlag = 0;
		iend=iend->next;
	
	}

	for(i=0; i<n; i++)
	{
		while(t)
		{
			if(strcmp(min,t->ID)>=0 && t->pFlag==0)
			{
				strcpy(min,t->ID);
				iend=t;
			
			}
			t=t->next;

		}
		
		printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",iend->ID,iend->name,iend->sex,iend->class,iend->tel,iend->adress,iend->grade);
		iend->pFlag = 1;
		strcpy(min, "9999999999999999999");	
	        t = head->next;
	}


	if(flag==2)
	{
		system("clear");
		menu1();
		return;
	}
	else
	{
		printf("请输入有效数字！\n");
	}

}
void cover_to_file()
{
	FILE *fp;

	STU p;
	p=(STU)malloc(sizeof(STUDENT)); //乱码问题牵扯到初始化，如果不加这句直接对p进行调用储存，就会乱码
	int i;
	system("clear");
	if((fp = fopen("information.txt","wt+"))==NULL)//at+
	{
		printf("\nCan't open the file!\n");
		
		exit(0);
	
	}
	for(p=head->next; p!=NULL; p=p->next)
	fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n",p->name,p->ID,p->sex,p->class,p->tel,p->adress,p->grade);
	printf("\n\t\tsave file success!\n");
	

}
//保存至文件（文件操作）
void save_to_file()
{
	
	FILE *fp;

	STU p;
	p=(STU)malloc(sizeof(STUDENT)); //乱码问题牵扯到初始化，如果不加这句直接对p进行调用储存，就会乱码
	int i;
	system("clear");
	if((fp = fopen("information.txt","at+"))==NULL)//at+
	{
		printf("\nCan't open the file!\n");
		
		exit(0);
	
	}
	for(p=head->next; p!=NULL; p=p->next)
	fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n",p->name,p->ID,p->sex,p->class,p->tel,p->adress,p->grade);
	printf("\n\t\tsave file success!\n");
	
/*	iend=head;
	while(iend)
	{
		p=iend->next;
		free(iend);
		iend=p->next;
	
	}
	head=NULL;*/
	
	return;
}
int put_order()
{
	STU p;
	int n=0;
	for(p=head->next; p!=NULL; p=p->next)
	{
	//	printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n",p->name,p->ID,p->sex,p->class,p->tel,p->adress,p->grade);
		n++;
	}
	
}

//退出系统
void exit_system()
{

	exit(0);

}


void menu1(void)
{
	system("clear");
	puts("\t\t\t\t************************应聘信息管理系统**********************\n\n");
	puts("\t\t\t\t                1.查看所有应聘者信息\n\n");
	puts("\t\t\t\t                2.查询信息（输入姓名）\n\n");
	puts("\t\t\t\t                3.信息排序 列出所有的应聘者记录（按学号顺序）\n\n");
	puts("\t\t\t\t                0.退出\n\n");
	puts("\t\t\t\t************************************************************************");
	puts("\t\t\t\t请输入功能序号(返回上一菜单请按 q )：");

}
void menu2(void)
{
	system("clear");
	puts("\t\t\t\t****************************应聘信息管理系统*******************************\n");
	puts("\t\t\t\t             1.应聘者信息录入\n");
	puts("\t\t\t\t             2.查找应聘者信息\n");
	puts("\t\t\t\t             3.删除应聘者信息\n");
	puts("\t\t\t\t             4.按成绩列出所有的应聘者记录\n");
	puts("\t\t\t\t             5.按序号列出所有的应聘者记录\n");
	puts("\t\t\t\t             6.查看历史应聘者记录\n");
	puts("\t\t\t\t             7.保存\n");
	puts("\t\t\t\t             0.退出\n");
	puts("\t\t\t\t*************************************************************************");
	puts("\t\t\t\t请输入功能序号（返回上一菜单请按 q ）：");
}

void menu(void)
{
	system("clear");
    puts("\t\t\t\t**********************应聘信息管理系统******************\n");
	puts("\t\t\t\t请选择你的身份:\n\n");
    puts("\t\t\t\t               1.应聘者\n");
    puts("\t\t\t\t               2.hr\n");
    puts("\t\t\t\t               3.管理员\n");
    puts("\t\t\t\t*******************************************************");
    puts("\t\t\t\t请输入功能序号（返回上一菜单请按 q ）：");

}

void login_student()
{
	char password1[100];
	char password[100];
	char username[100];
	char username1[100];
	printf("\n\n\n\n\t\t\t\tusername:");
	scanf("%s",username1);
	printf("\n\n\t\t\t\tpassword:");
	scanf("%s",password1);

	FILE *pf,*pf1;
	ORD s;
	if((pf = fopen("student_order.txt","at+"))==NULL)
	{
		printf("error! 读取文件出错");
		exit(1);
	}

	head_student=(ORD)malloc(sizeof(ORDER));
	head_student->next=NULL;
	iend_student=head_student;

	while(!feof(pf))
	{
		s=(ORD)malloc(sizeof(ORDER));
		fscanf(pf,"%s\t%s",s->username,s->password);
		iend_student->next=s;
		s->pre=iend_student;
		iend_student=s;
	}
	iend_student->next=NULL;
	fclose(pf);

	int flag=0;
	iend_student=(ORD)malloc(sizeof(STUDENT));
	iend_student=head_student->next;

	while(iend_student)
	{	
		if(strcmp(iend_student->username,username1)==0)
		{
			flag=1;
			if(strcmp(iend_student->password,password1)==0)
			{
				system("clear");
				menu1_system();
				break;	
			}
		}
		iend_student=iend_student->next;
		
	}

	if(flag ==0)
	{
  		printf("没有？创建一个吧那么~");
		pf1=fopen("student_order.txt","at+");
		s=(ORD)malloc(sizeof(ORDER));

		printf("new username:");
		scanf("%s",s->username);
		printf("new password");
		scanf("%s",s->password);

		fprintf(pf1,"%s\t%s",s->username,s->password);
		printf("录入成功！");

		printf("返回登录界面请按 q ");
		int q;
		scanf("%c",&q);
		if(q=='q')
		login_student();
	}

	return;
    
}

void login_hr()
{
	char password1[100];
	char password[100];
	char username[100];
	char username1[100];
	printf("\n\n\n\n\t\t\t\tusername:");
	scanf("%s",username1);
	printf("\n\npassword:");
	scanf("%s",password1);

	FILE *pf,*pf1;
	ORD s;
	if((pf = fopen("hr_order.txt","r+"))==NULL)
	{
		printf("error! 读取文件出错");
		exit(1);
	}

	head_hr=(ORD)malloc(sizeof(ORDER));
	head_hr->next=NULL;
	iend_hr=head_hr;
	
	while(!feof(pf))
	{
		s=(ORD)malloc(sizeof(ORDER));
		fscanf(pf,"%s\t%s",s->username,s->password);
		iend_hr->next=s;
		s->pre=iend_hr;
		iend_hr=s;
	}
	iend_hr->next=NULL;
	fclose(pf);

	int flag=0;
	iend_hr=(ORD)malloc(sizeof(ORDER));
	iend_hr=head_hr->next;
	
	while(iend_hr)
	{	
		if(strcmp(iend_hr->username,username1)==0)
		{
			flag=1;
			if(strcmp(iend_hr->password,password1)==0)
			 { 
				 system("clear");
				 menu2_system();
				 break;
			 
			 }
		}
		iend_hr=iend_hr->next;
		

	}
	if(flag ==0)
	{
  		printf("没有？那么创建一个吧~");
		pf1=fopen("hr_order.txt","at+");
		s=(ORD)malloc(sizeof(ORDER));
		printf("new username:");
		scanf("%s",s->username);
		printf("new password");
		fprintf(pf1,"%s\t%s",s->username,s->password);
		printf("录入成功！");
		printf("返回登录界面请按 q ");
		int q;
		scanf("%c",&q);
		if(q=='q')
		login_hr();
	}

	return ;
      

}

void menu1_system(void)
{
	// login();//先进行学生账号登录 
	 char choice;
	 int n;
	
	while(1)
	{
		menu1();
		scanf("%c",&choice);
		if(choice=='q')
		break;

		switch(choice - '0' )
		{

			case 1:   read_file();
				  break;
			case 2:   search_student();
				  break;
		
			case 3:   print_student_number_order();
				  break;
		
			case 0:   exit_system();exit(0);
				  break;
		
			default:  break;
		
		}
		
	
	}
	//menu();
	return;

}
void menu2_system()
{
	char choice;
	int n;
	
	while(1)
	{
		menu2();
		scanf("%c",&choice);
		if(choice=='q')
		break;

		switch(choice - '0' )
		{

			case 1:   insert_student_information();
				  break;
			case 2:   search_student();
				  break;
			case 3:   delete_student();
				  break;
			case 4:   print_student_addition_order();
				  break;
			case 5:   print_student_number_order();
				  break;
			case 6:   read_file();
				  break;
			case 7:   save_to_file();
			      break;	
		//	case 8: menu(); return;//返回上一层页面//
			case 0:   exit_system();exit(0);
				  break;
		
			default:  break;
		
		}
		
	
	}
	//menu();
	return;

}
void menu_system()
{
	char choice; 
	
		while(1)
	{
		menu();
 	    scanf("%c",&choice);
		if(choice == 'q') break;
 	    switch(choice -'0')
  	    {
 	        case 1: login_student(); break;
			case 2: login_hr(); break;
		//	case 3: menu3(); break;
			default: break;
 	   }

	}
	return;

}

//登录界面
void login_system()
{
	char ch;
	printf("\t\t\t\t*********************welcome to *********************\n\n\n\n\n");
	printf("\t\t\t\t            welcome to candidate system              \n");
	printf("\t\t\t\t            Reading instructions(press ‘h’)\n\n\n\n");
	printf("\t\t\t\t*****************************************************\n");
	printf("\t\t\t\tplease press the Enter to in");
	scanf("%c",&ch);
	if(ch == '\n')
	menu_system();//选择身份
	else if(ch == 'h' || ch == 'H')
	help_system();
	else
	printf("error! please entery the right key");


	//admin_student(head_student);
	//admin_hr(head_hr);
	//menu_system();//选择身份
}

void admin_student()
{
	char username1[100];
	char password1[100];
	head_student=(ORD)malloc(sizeof(ORDER));
	char choice;
	do
	{
		printf("1.添加用户\n");
		printf("2.更改用户密码\n");
		printf("3.删除用户\n");
		printf("4.退出\n");
		printf("请输入功能序号（按q返回上一层）：");

		scanf("%c",choice);

		switch(choice -'0')
		{
			case 1:    //添加用户 
			       break;
			case 2:      //更改用户密码
			       break;
			case 3:      //删除用户
			       break;
			case 4:exit(0);//退出
			default:break;
		}

	}while(choice != 'q');

	printf("\t\t\t\tplease login：\n");
	printf("\t\t\t\tusername:");
	scanf("%s",username1);
	printf("\n\t\t\t\tpassword:");
	scanf("%s",password1);
	
}

void help_system()//说明书 稍后完善
{
	char ch;
	puts("     instructions");
	puts("............待补充");
	scanf("%c",ch);
	if(ch == 'q')
	return ;

}


int main(int argc, char const *argv[])
{
	int choice;
	int n;
	head=(STU)malloc(sizeof(STUDENT)); //最开始的head要记得初始化
	head->next=NULL;
	login_system();

/*	while(1)
	{
		menu();
 	    scanf("%d",&choice);
 	    switch(choice)
  	    {
 	        case 1: menu1_system(); menu(); break;
		// 	case 2: menu2(); break;
		//	case 3: menu3(); break;
			default: break;
 	   }

	}
	
*/

/*	while(1)
	{
		scanf("%d",&choice);
		
		switch(choice )
		{

			case 1:   insert_student_information(n);
				  break;
			case 2:   search_student();
				  break;
			case 3:   delete_student();
				  break;
			case 4:   print_student_addition_order();
				  break;
			case 5:   print_student_number_order();
				  break;
			case 6:   read_file();
				  break;
			case 7:   save_to_file();
			      break;
			case 8:   break;
			case 9:   exit_system();exit(0);
				  break;
			default:  break;
		
		}
		
	
	}*/

	return 0;

}

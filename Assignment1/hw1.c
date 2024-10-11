#include <stdio.h>
#include <stdlib.h> // 動態記憶體配置
#include <string.h> // 可以使用字串相關函式

//打算使用鍊表串聯起多個key-value

// 定義資料長度
#define KEY_SIZE 15
#define PHONE_SIZE 15
#define ADDRESS_SIZE 50
#define EMAIL_SIZE 30
#define FILE_NAME "contact.txt"  //存成檔名為contact的txt檔

// 存取多個VALUE的結構，就是通訊錄的資料
typedef struct {
    char phone[PHONE_SIZE];
    char address[ADDRESS_SIZE];
    char email[EMAIL_SIZE];
} Contact;

// 聯絡人的結構體，包含聯絡人的key和資料，還有指向下一個聯絡人的指標
typedef struct Name {
    char key[KEY_SIZE]; // 聯絡人的名字
    Contact contact;    // 聯絡人的通訊資料
    struct Name *next;  // 指向下一個聯絡人結點的指標
} Name;

Name* contactInfo = NULL; // 鍊表的頭

// 新增/更新聯絡人
void addnew(const char *key, const char *phone, const char *address, const char *email) {
    // 出現過新增空主鍵的BUG，這段可防止空資料被新增
    //strlen字串長度等於0時，不能用
    if (strlen(key) == 0 || strlen(phone) == 0 || strlen(address) == 0 || strlen(email) == 0) {
        printf("資料無效，聯絡人未新增。\n");
        return;
    }

    Name *name = contactInfo;//*name指向聯絡人鏈表的第一個節點（也就是目前的鏈表開頭）
    Name *prev = NULL;//指向當前name的前一個節點。當name遍歷鏈表時，prev用來記錄前一個聯絡人。

    // 檢查是否已有相同人存在
    //不過若有同名同姓，這個功能就沒用了
    while (name != NULL) {//name->key == *name.key
        if (strcmp(name->key, key) == 0) {//strcmp檢查是否出現一樣的人(等於0)
            // 更新聯絡人資料
            strcpy(name->contact.phone, phone);//*name.contact.phone
            strcpy(name->contact.address, address);
            strcpy(name->contact.email, email);
            return;
        }
        prev = name;//指向下一個node
        name = name->next;//一樣是指向下一個node，才可以繼續往下搜尋
    }

    // 創建一個新的聯絡人節點
    name = (Name *)malloc(sizeof(Name));//malloc分配記憶體給聯絡人node
    if (name == NULL) {
        fprintf(stderr, "記憶體分配失敗\n");//檢查是否有分配
        exit(EXIT_FAILURE);
    }
    strcpy(name->key, key);//字串複製
    strcpy(name->contact.phone, phone);
    strcpy(name->contact.address, address);
    strcpy(name->contact.email, email);
    name->next = NULL;


    // 插入鏈表的末端
    if (prev == NULL) {
        contactInfo = name;// 若都沒有資料第一個節點
    } else {
        prev->next = name;//有的話就放最後一個
    }
}

// 查詢聯絡人
Name* get(const char *key) {
    Name *name = contactInfo;


    // 遍歷鏈表尋找聯絡人
    while (name != NULL) {
        if (strcmp(name->key, key) == 0) {//等於0為有找到
            return name;
        }
        name = name->next;
    }

    return NULL;// 找不到聯絡人
}

// 刪除聯絡人
void delete(const char *key) {
    Name *name = contactInfo;
    Name *prev = NULL;


    // 跑一次鏈表尋找要刪除的聯絡人
    while (name != NULL) {
        if (strcmp(name->key, key) == 0) {//先找到該筆資料
            if (prev == NULL) {
                contactInfo = name->next;
            } else {
                prev->next = name->next;
            }
            free(name);  // 刪除了便釋放記憶體
            return;
        }
        prev = name;
        name = name->next;//指向下一個node
    }
}

// 顯示所有聯絡人
void allInfo() {
    Name *name = contactInfo;
    if (name == NULL) {
        printf("沒有聯絡人可顯示。\n");
        return;
    }
    while (name != NULL) {
        printf("Name: %s\n", name->key);
        printf("Phone: %s\n", name->contact.phone);
        printf("Address: %s\n", name->contact.address);
        printf("Email: %s\n", name->contact.email);
        printf("---------------\n");
        name = name->next;//往下一個node
    }
}

// 將輸入的資料寫入到TXT檔，並以JSON格式儲存
void saveFile() {
    FILE *fptr = fopen(FILE_NAME, "w"); // 用ws模式開啟，不會覆蓋原有檔案
    if (fptr == NULL) {
        fprintf(stderr, "無法開啟檔案\n");//若無法正常開啟或新增會立刻顯示
        return;
    }

    fprintf(fptr, "{\n");
    Name *name = contactInfo;//走遍整個鍊表
    while (name != NULL) {//開始用Json格式存
        fprintf(fptr, "    \"%s\": {\n", name->key);//一個tab
        fprintf(fptr, "        \"phone\": \"%s\",\n", name->contact.phone);//下面都兩個tab
        fprintf(fptr, "        \"address\": \"%s\",\n", name->contact.address);
        fprintf(fptr, "        \"email\": \"%s\"\n", name->contact.email);

        name = name->next;//往下一個

        if (name != NULL) {//看是不是最後一位，決定要不要加逗號
            fprintf(fptr, "    },\n");// 結束這個聯絡人時，並加上逗號
        } else {
            fprintf(fptr, "    }\n");// 最後一個聯絡人不用逗號
        }
    }
    fprintf(fptr, "}\n");// 結束 JSON
    fclose(fptr);
}

//讀取以存在的檔案，查詢已存在的資料(與使用中時輸入的不一樣)
void loadFile() {
    FILE *fptr = fopen(FILE_NAME, "r");// 以讀取模式打開文件
    if (fptr == NULL) {// 檔案打開失敗
        fprintf(stderr, "沒有檔案\n");
        return;// 檔案不存在，直接返回
    }

    char key[KEY_SIZE], phone[PHONE_SIZE], address[ADDRESS_SIZE], email[EMAIL_SIZE];

    //跳過開頭的'{'
    fscanf(fptr, " {\n");

    //從檔案中讀取每個聯絡人
    while (fscanf(fptr, " \"%[^\"]\": {\n", key) == 1) {//[^\"]讀取所有非"符號的字元
        fscanf(fptr, " \"phone\": \"%[^\"]\",\n", phone);
        fscanf(fptr, " \"address\": \"%[^\"]\",\n", address);
        fscanf(fptr, " \"email\": \"%[^\"]\"\n", email);

        //將讀取到的資料儲存到鏈表中
        addnew(key, phone, address, email);
        //跳過結束符號},或}
        fscanf(fptr, " },\n");
    }
    fclose(fptr);
}


void freeMem() {//釋放配置的記憶體
    Name *name = contactInfo;
    while (name != NULL) {
        Name *temp = name;
        name = name->next;
        free(temp);
    }
}


void Menu() {//使用者決定功能
    int choice;
    char key[KEY_SIZE], phone[PHONE_SIZE], address[ADDRESS_SIZE], email[EMAIL_SIZE];//存輸入的資料

    while (1) {
        printf("======通訊錄======\n");
        printf("1.新增/更新聯絡人\n");
        printf("2.查詢聯絡人\n");
        printf("3.刪除聯絡人\n");
        printf("4.顯示所有聯絡人\n");
        printf("5.退出並保存\n");
        printf("請選擇操作：");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("輸入聯絡人名稱：");
                scanf("%s", key);
                printf("輸入電話號碼：");
                scanf("%s", phone);
                printf("輸入地址：");
                scanf("%s", address);
                printf("輸入電子信箱：");
                scanf("%s", email);
                addnew(key, phone, address, email);
                printf("聯絡人已新增/更新\n");
                break;
            case 2:
                printf("輸入聯絡人名稱：");
                scanf("%s", key);
                Name *find = get(key);
                if (find) {
                    printf("Phone: %s\n", find->contact.phone);
                    printf("Address: %s\n", find->contact.address);
                    printf("Email: %s\n", find->contact.email);
                } else {
                    printf("找不到此聯絡人\n");
                }
                break;
            case 3:
                printf("輸入要刪除的聯絡人名稱：");
                scanf("%s", key);
                delete(key);
                printf("聯絡人已刪除（若存在）\n");
                break;
            case 4:
                allInfo();
                break;
            case 5:
                saveFile();
                freeMem();
                printf("資料已保存並退出程式\n");
                return;
            default:
                printf("無效的選擇，請重新輸入\n");
        }
    }
}

// 主程式
int main() {
    loadFile();  // 從檔案加載資料
    Menu();  // 進入選項菜單
    return 0;
}

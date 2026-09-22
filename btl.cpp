#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX 1000
#define INF 1e9

typedef struct Route {
    int routeId;
    int dest;
    char streetName[50];
    float length;
    float time;
    struct Route *next;
} Route;

typedef struct Location {
    int id;
    char name[50];
    Route *routeList;
    struct Location *next;
} Location;

Location *head = NULL;
int locationCount = 0;
int routeCount = 0;

Location* findLocation(int id) {
    Location *p = head;
    while (p) {
        if (p->id == id) return p;
        p = p->next;
    }
    return NULL;
}

int addLocation(int id, const char *name) {
    if (findLocation(id)) return 0;
    Location *p = (Location*)malloc(sizeof(Location));
    p->id = id;
    strncpy(p->name, name, 49);
    p->name[49] = '\0';
    p->routeList = NULL;
    p->next = NULL;

    if (!head) {
        head = p;
    } else {
        Location *q = head;
        while (q->next) q = q->next;
        q->next = p;
    }
    locationCount++;
    return 1;
}

void delRoute(Route **list, int dest) {
    Route *p = *list, *prev = NULL;
    while (p) {
        if (p->dest == dest) {
            Route *tmp = p;
            if (prev) prev->next = p->next;
            else *list = p->next;
            p = p->next;
            free(tmp);
        } else {
            prev = p;
            p = p->next;
        }
    }
}

int delLocation(int id) {
    Location *p = head, *prev = NULL;
    while (p) {
        if (p->id == id) break;
        prev = p;
        p = p->next;
    }
    if (!p) return 0;

    if (prev) prev->next = p->next;
    else head = p->next;

    Route *t = p->routeList;
    while (t) {
        Route *tmp = t;
        t = t->next;
        free(tmp);
    }
    free(p);
    locationCount--;

    Location *q = head;
    while (q) {
        delRoute(&q->routeList, id);
        q = q->next;
    }
    return 1;
}

int addRoute(int u, int v, const char *streetName,
             float length, float time) {
    Location *du = findLocation(u);
    Location *dv = findLocation(v);
    if (!du || !dv) return 0;

    Route *t = (Route*)malloc(sizeof(Route));
    t->routeId = ++routeCount;
    t->dest = v;
    strncpy(t->streetName, streetName, 49);
    t->streetName[49] = '\0';
    t->length = length;
    t->time = time;
    t->next = NULL;

    if (!du->routeList) {
        du->routeList = t;
    } else {
        Route *p = du->routeList;
        while (p->next) p = p->next;
        p->next = t;
    }

    if (u != v) {
        Route *t2 = (Route*)malloc(sizeof(Route));
        t2->routeId = t->routeId;
        t2->dest = u;
        strncpy(t2->streetName, streetName, 49);
        t2->streetName[49] = '\0';
        t2->length = length;
        t2->time = time;
        t2->next = NULL;

        if (!dv->routeList) dv->routeList = t2;
        else {
            Route *p = dv->routeList;
            while (p->next) p = p->next;
            p->next = t2;
        }
    }
    return 1;
}

int delRouteByPair(int u, int v) {
    Location *du = findLocation(u);
    Location *dv = findLocation(v);
    if (!du || !dv) return 0;

    int found = 0;
    Route *p = du->routeList, *prev = NULL;
    while (p) {
        if (p->dest == v) {
            Route *tmp = p;
            if (prev) prev->next = p->next;
            else du->routeList = p->next;
            p = p->next;
            free(tmp);
            found = 1;
            break;
        } else {
            prev = p;
            p = p->next;
        }
    }
    if (!found) return 0;

    p = dv->routeList; prev = NULL;
    while (p) {
        if (p->dest == u) {
            Route *tmp = p;
            if (prev) prev->next = p->next;
            else dv->routeList = p->next;
            p = p->next;
            free(tmp);
            break;
        } else {
            prev = p;
            p = p->next;
        }
    }
    return 1;
}

void showLoc() {
    printf("\n--- DANH SACH DIA DIEM ---\n");
    Location *p = head;
    while (p) {
        printf("[%d] %s\n", p->id, p->name);
        p = p->next;
    }
}

void showRoute() {
    printf("\n--- DANH SACH TUYEN DUONG ---\n");
    Location *p = head;
    while (p) {
        Route *t = p->routeList;
        while (t) {
            if (p->id < t->dest) {
                printf("%d <-> %d | %s | %.2f km | %.2f phut\n",
                       p->id, t->dest, t->streetName,
                       t->length, t->time);
            }
            t = t->next;
        }
        p = p->next;
    }
}

void BFS(int s, int *visited, int *parent) {
    int queue[MAX], front = 0, rear = 0;
    for (int i = 0; i < MAX; i++) { visited[i] = 0; parent[i] = -1; }

    Location *ds = findLocation(s);
    if (!ds) return;

    queue[rear++] = s;
    visited[s] = 1;

    while (front < rear) {
        int u = queue[front++];
        Location *du = findLocation(u);
        for (Route *t = du->routeList; t; t = t->next) {
            if (!visited[t->dest]) {
                visited[t->dest] = 1;
                parent[t->dest] = u;
                queue[rear++] = t->dest;
            }
        }
    }
}

void dfs(Location *u, int *visited) {
    if (!u) return;
    visited[u->id] = 1;
    printf("%d(%s) ", u->id, u->name);
    for (Route *t = u->routeList; t; t = t->next) {
        if (!visited[t->dest]) {
            Location *v = findLocation(t->dest);
            dfs(v, visited);
        }
    }
}

int isLinked(int u, int v) {
    int visited[MAX], parent[MAX];
    BFS(u, visited, parent);
    return visited[v];
}

void listReach(int u) {
    int visited[MAX], parent[MAX];
    BFS(u, visited, parent);
    printf("Cac dia diem co the di den tu %d:\n", u);
    Location *p = head;
    while (p) {
        if (p->id != u && visited[p->id])
            printf("  -> [%d] %s\n", p->id, p->name);
        p = p->next;
    }
}

void bfsPath(int s, int t) {
    int visited[MAX], parent[MAX];
    BFS(s, visited, parent);

    if (!visited[t]) {
        printf("Khong co duong di tu %d den %d\n", s, t);
        return;
    }

    int path[MAX], len = 0;
    int cur = t;
    while (cur != -1) {
        path[len++] = cur;
        cur = parent[cur];
    }
    printf("Duong di it canh nhat tu %d den %d:\n", s, t);
    for (int i = len - 1; i >= 0; i--) {
        Location *d = findLocation(path[i]);
        printf("%d(%s)", path[i], d ? d->name : "?");
        if (i > 0) printf(" -> ");
    }
    printf("\nSo canh: %d\n", len - 1);
}

void dij(int s, int t) {
    float dist[MAX];
    int visited[MAX], parent[MAX];
    for (int i = 0; i < MAX; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }
    dist[s] = 0;

    for (int k = 0; k < locationCount; k++) {
        int u = -1;
        float best = INF;
        Location *p = head;
        while (p) {
            if (!visited[p->id] && dist[p->id] < best) {
                best = dist[p->id];
                u = p->id;
            }
            p = p->next;
        }
        if (u == -1) break;
        visited[u] = 1;
        if (u == t) break;

        Location *du = findLocation(u);
        for (Route *e = du->routeList; e; e = e->next) {
            if (dist[e->dest] > dist[u] + e->length) {
                dist[e->dest] = dist[u] + e->length;
                parent[e->dest] = u;
            }
        }
    }

    if (dist[t] == INF) {
        printf("Khong co duong di tu %d den %d\n", s, t);
        return;
    }
    int path[MAX], len = 0, cur = t;
    while (cur != -1) {
        path[len++] = cur;
        cur = parent[cur];
    }
    printf("Duong di ngan nhat (theo do dai) tu %d den %d:\n", s, t);
    for (int i = len - 1; i >= 0; i--) {
        Location *d = findLocation(path[i]);
        printf("%d(%s)", path[i], d ? d->name : "?");
        if (i > 0) printf(" -> ");
    }
    printf("\nTong do dai: %.2f km\n", dist[t]);
}

void tree(Location *u, int *parent, int *visited, int depth, int *isChild) {
    if (!u) return;
    for (int i = 0; i < depth; i++) printf("    ");
    if (depth == 0) printf("[GOC] %d(%s)\n", u->id, u->name);
    else printf("|-- %d(%s)\n", u->id, u->name);

    Location *p = head;
    while (p) {
        if (parent[p->id] == u->id && !visited[p->id]) {
            visited[p->id] = 1;
            tree(p, parent, visited, depth + 1, isChild);
        }
        p = p->next;
    }
}

void bfsTree(int root) {
    int visited[MAX], parent[MAX];
    BFS(root, visited, parent);

    printf("\n--- CAY BFS TU GOC %d ---\n", root);
    int mark[MAX] = {0};
    Location *r = findLocation(root);
    mark[root] = 1;
    tree(r, parent, mark, 0, NULL);

    Location *p = head;
    int orphanCount = 0;
    while (p) {
        if (!visited[p->id]) {
            if (orphanCount == 0) printf("\n(Cac dinh khong lien thong voi goc)\n");
            printf("[ROI] %d(%s)\n", p->id, p->name);
            orphanCount++;
        }
        p = p->next;
    }
}

int pathTrace[MAX];
int visitedBT[MAX];
int pathFoundCount = 0;

void backtrack(int u, int target, float k, int depth, float totalLength) {
    pathTrace[depth] = u;

    if (u == target) {
        pathFoundCount++;
        printf("Duong %d: ", pathFoundCount);
        for (int i = 0; i <= depth; i++) {
            Location *d = findLocation(pathTrace[i]);
            printf("%d(%s)", pathTrace[i], d ? d->name : "?");
            if (i < depth) printf(" -> ");
        }
        printf(" | Tong do dai = %.2f\n", totalLength);
        return;
    }

    Location *du = findLocation(u);
    for (Route *t = du->routeList; t; t = t->next) {
        if (!visitedBT[t->dest]) {
            float newLen = totalLength + t->length;
            if (newLen <= k) {
                visitedBT[t->dest] = 1;
                backtrack(t->dest, target, k, depth + 1, newLen);
                visitedBT[t->dest] = 0;
            }
        }
    }
}

void allPaths(int s, int t, float k) {
    if (!findLocation(s) || !findLocation(t)) {
        printf("Dia diem khong ton tai!\n");
        return;
    }
    for (int i = 0; i < MAX; i++) visitedBT[i] = 0;
    visitedBT[s] = 1;
    pathFoundCount = 0;
    printf("\n--- TAT CA DUONG DI TU %d DEN %d CO DO DAI <= %.2f ---\n", s, t, k);
    backtrack(s, t, k, 0, 0.0f);
    if (pathFoundCount == 0) printf("Khong tim thay duong di nao thoa man.\n");
    else printf("Tong cong: %d duong\n", pathFoundCount);
}

int readFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Khong mo duoc file %s\n", filename);
        return 0;
    }

    while (head) {
        Location *tmp = head;
        head = head->next;
        Route *t = tmp->routeList;
        while (t) {
            Route *tt = t;
            t = t->next;
            free(tt);
        }
        free(tmp);
    }
    locationCount = 0;
    routeCount = 0;

    int n;
    if (fscanf(f, "%d", &n) != 1) { fclose(f); return 0; }

    for (int i = 0; i < n; i++) {
        int id; char name[50];
        fscanf(f, "%d %s", &id, name);
        addLocation(id, name);
    }

    int m;
    if (fscanf(f, "%d", &m) != 1) { fclose(f); return 1; }
    for (int i = 0; i < m; i++) {
        int u, v;
        char streetName[50];
        float length, time;
        fscanf(f, "%d %d %s %f %f", &u, &v, streetName, &length, &time);
        addRoute(u, v, streetName, length, time);
    }
    fclose(f);
    printf("Da doc file %s: %d dia diem, %d tuyen.\n", filename, n, m);
    return 1;
}

void menu() {
    printf("\n============= QUAN LY DUONG DI DO THI =============\n");
    printf(" 1. Them dia diem\n");
    printf(" 2. Xoa dia diem\n");
    printf(" 3. Them tuyen duong\n");
    printf(" 4. Xoa tuyen duong\n");
    printf(" 5. Kiem tra hai dia diem co ket noi\n");
    printf(" 6. Liet ke cac dia diem co the di den\n");
    printf(" 7. Tim duong di it canh nhat (BFS)\n");
    printf(" 8. Tim duong di ngan nhat theo do dai (Dijkstra)\n");
    printf(" 9. Hien thi cay BFS tu goc\n");
    printf("10. Tim tat ca duong di <= k (quay lui)\n");
    printf("11. In danh sach dia diem\n");
    printf("12. In danh sach tuyen duong\n");
    printf("13. Doc lai file dothi.txt\n");
    printf(" 0. Thoat\n");
    printf("===================================================\n");
    printf("Chon: ");
}

int main() {
    readFile("dothi.txt");

    int choice;
    do {
        menu();
        scanf("%d", &choice);
        switch (choice) {

        case 1: {
            int id; char name[50];
            printf("Nhap id: "); scanf("%d", &id);
            printf("Nhap ten: "); scanf("%s", name);
            if (addLocation(id, name)) printf("Them thanh cong.\n");
            else printf("Id da ton tai!\n");
            break;
        }
        case 2: {
            int id;
            printf("Nhap id can xoa: "); scanf("%d", &id);
            if (delLocation(id)) printf("Xoa thanh cong.\n");
            else printf("Khong tim thay id.\n");
            break;
        }
        case 3: {
            int u, v; char name[50]; float d, t;
            printf("Nhap u v: "); scanf("%d %d", &u, &v);
            printf("Ten duong: "); scanf("%s", name);
            printf("Do dai (km): "); scanf("%f", &d);
            printf("Thoi gian (phut): "); scanf("%f", &t);
            if (addRoute(u, v, name, d, t)) printf("Them tuyen thanh cong.\n");
            else printf("Dia diem khong ton tai.\n");
            break;
        }
        case 4: {
            int u, v;
            printf("Nhap u v: "); scanf("%d %d", &u, &v);
            if (delRouteByPair(u, v)) printf("Xoa tuyen thanh cong.\n");
            else printf("Khong tim thay tuyen.\n");
            break;
        }
        case 5: {
            int u, v;
            printf("Nhap u v: "); scanf("%d %d", &u, &v);
            if (isLinked(u, v)) printf("CO ket noi.\n");
            else printf("KHONG ket noi.\n");
            break;
        }
        case 6: {
            int u;
            printf("Nhap dia diem xuat phat: "); scanf("%d", &u);
            listReach(u);
            break;
        }
        case 7: {
            int s, t;
            printf("Nhap s t: "); scanf("%d %d", &s, &t);
            bfsPath(s, t);
            break;
        }
        case 8: {
            int s, t;
            printf("Nhap s t: "); scanf("%d %d", &s, &t);
            dij(s, t);
            break;
        }
        case 9: {
            int root;
            printf("Nhap goc: "); scanf("%d", &root);
            if (findLocation(root)) bfsTree(root);
            else printf("Khong ton tai goc.\n");
            break;
        }
        case 10: {
            int s, t; float k;
            printf("Nhap s t: "); scanf("%d %d", &s, &t);
            printf("Nhap k (do dai toi da): "); scanf("%f", &k);
            allPaths(s, t, k);
            break;
        }
        case 11:
            showLoc();
            break;
        case 12:
            showRoute();
            break;
        case 13:
            readFile("dothi.txt");
            break;
        case 0:
            printf("Thoat chuong trinh.\n");
            break;
        default:
            printf("Lua chon khong hop le.\n");
        }
    } while (choice != 0);

    while (head) {
        Location *tmp = head;
        head = head->next;
        Route *t = tmp->routeList;
        while (t) {
            Route *tt = t;
            t = t->next;
            free(tt);
        }
        free(tmp);
    }
    return 0;
}
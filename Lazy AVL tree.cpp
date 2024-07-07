#include<iostream>
#include<algorithm>
#include<cmath>

using namespace std;

typedef struct node_t{
	node_t *parent;
	node_t *left;
	node_t *right;
	int value;
	int d_left;
	int d_right;
	int is_deleted;
	int deleted_cnt;
} node_t;

int D = 0, R = 0, C = 0, limit = 0;
node_t *root = NULL;

void insert(int num);
node_t *create_new_node(node_t *parent,  int value);
void del(int num);
void consolidation(node_t *subroot);
void remove(node_t *node);
node_t *find_max(node_t *node);
node_t *find_min(node_t *node);
void update_dltd_cnt(node_t *node);
node_t *going_up(node_t *leaf, int is_inserted);
int compute_rotation(node_t *nd);
node_t *r_rotation(node_t *node);
node_t *l_rotation(node_t *node);
void print_tree(node_t *nd);

int main(){
	int N, cmd;
	scanf("%d", &N);
	for(int i = 0;i < N;i++){
		scanf("%d", &cmd);
		if(cmd > 0)
			insert(cmd);
		else
			del(-1 * cmd);

		if(root == NULL){
			D = -1;
			continue;
		}
		else if(root->d_left == 0 && root->d_right == 0)
			D = -1;
		else
			D = max(root->d_left, root->d_right);

		limit = 1 + D/2;
		// printf("\n%d >= %d when D = %d?\n", root->deleted_cnt,  limit, D);
		if(root->deleted_cnt >= limit){
			C++;
			while(root->deleted_cnt != 0){
				// print_tree(root);
				consolidation(root);
				if(root == NULL)
					break;
				
			}
		}
		// print_tree(root);
	}

	if(root == NULL)
		D = -1;
	else if(root->d_left == 0 && root->d_right == 0)
		D = -1;
	else
		D = max(root->d_left, root->d_right);
	printf("%d %d %d\n", D, R, C);
}

void insert(int num){
	node_t *nd = root;
	node_t *parent = NULL;

	if(nd == NULL){
		root = create_new_node(parent, num);
		return;
	}

	// get lowest parent
	while(nd != NULL){
		parent = nd;
		if(nd->value < num)
			nd = nd->right;
		else if(nd->value > num)
			nd = nd->left;
		else{
			if(nd->is_deleted){
				nd->deleted_cnt--;
				nd->is_deleted = 0;
				update_dltd_cnt(nd);
			}
			return;
		}
	}

	nd = create_new_node(parent, num);
	if(parent != NULL){
		if(nd->value < parent->value){
			parent->left = nd;
			parent->d_left++;
		}
		else{
			parent->right = nd;
			parent->d_right++;
		}
	}

	root = going_up(nd, 1);
}

node_t *create_new_node(node_t *parent,  int value){
	node_t *node = (node_t*)malloc(sizeof(node_t));
	node->parent = parent;
	node->left = NULL;
	node->right = NULL;
	node->value = value;
	node->d_left = 0;
	node->d_right = 0;
	node->deleted_cnt = 0;
	node->is_deleted = 0;
	return node;
}

void del(int num){
	// change state of the node
	node_t *nd = root;

	if(nd == NULL)
		return;

	while(nd->value != num){
		if(nd->value < num)
			nd = nd->right;
		else
			nd = nd->left;
		if(nd == NULL)
			return;
	}

	if(nd->is_deleted == 0){
		nd->is_deleted = 1;
		nd->deleted_cnt++;
		update_dltd_cnt(nd);
	}
}

void consolidation(node_t *subroot){
	node_t *node = subroot;
	while(!(node->deleted_cnt == 1 && node->is_deleted)){
		if(node->left != NULL){
			if((node->left)->deleted_cnt != 0){
				node = node->left;
				continue;
			}
		}
		if(node->right != NULL && node->left != node){
			if((node->right)->deleted_cnt != 0){
				node = node->right;
				continue;
			}
		}
		
		return;
	}

	remove(node);
}

void remove(node_t *node){
	if(node->left == NULL && node->right == NULL){
		node_t *parent = node->parent;
		if(parent == NULL){
			root = NULL;
			return;
		}

		if(parent->left == NULL){
			parent->right = NULL;
			parent->d_right = 0;
		}
		else if(parent->right == NULL){
			parent->left = NULL;
			parent->d_left = 0;
		}
		else if((parent->left)->value == node->value){
			parent->left = NULL;
			parent->d_left = 0;
		}
		else if((parent->right)->value == node->value){
			parent->right = NULL;
			parent->d_right = 0;
		}


		if(parent->deleted_cnt > 0){
			parent->deleted_cnt--;
		}

		going_up(parent, 0);
	}
	else if(node->left != NULL){
		node_t *sub_max = find_max(node->left);
		node->value = sub_max->value;
		
		node->is_deleted = 0;
		remove(sub_max);
	}
	else if(node->right != NULL){
		node_t *sub_min = find_min(node->right);
		node->value = sub_min->value;
		
		node->is_deleted = 0;
		remove(sub_min);
	}
}

node_t *find_max(node_t *node){
	while(node->right != NULL){
		node = node->right;
	}
	return node;
}

node_t *find_min(node_t *node){
	while(node->left != NULL){
		node = node->left;
	}
	return node;
}

void update_dltd_cnt(node_t *node){
	int cur = 0;

	while(node->parent != NULL){
		cur = node->value;
		node = node->parent;
		if(cur < node->value){
			if(node->right != NULL)
				node->deleted_cnt = max((node->left)->deleted_cnt, (node->right)->deleted_cnt) + node->is_deleted;
			else
				node->deleted_cnt = (node->left)->deleted_cnt + node->is_deleted;
		}
		else{
			if(node->left != NULL)
				node->deleted_cnt = max((node->right)->deleted_cnt, (node->left)->deleted_cnt)  + node->is_deleted;
			else
				node->deleted_cnt = (node->right)->deleted_cnt + node->is_deleted;
		}
	}
}

node_t *going_up(node_t *leaf, int is_inserted){
	// we go up the tree from the leaf to the root updating d_left and d_right by diff value
	// in each node on this way. if they differ on more than 1, then we need to make rotations here
	// compute, what rotations are needed (use the last two nodes before this node)
	// make rotations (call r_rotation() and l_rotation())
	// return current root of the AVL tree 
	node_t *ret = root;
	node_t *nd = leaf;

	for(int i = 0;nd != NULL;i++){
		if(nd->left != NULL)
			nd->d_left = max((nd->left)->d_left, (nd->left)->d_right) + 1;
		else
			nd->d_left = 0;

		if(nd->right != NULL)
			nd->d_right = max((nd->right)->d_left, (nd->right)->d_right) + 1;
		else
			nd->d_right = 0;

		if(nd->left == NULL && nd->right == NULL)
			nd->deleted_cnt = nd->is_deleted;
		else if(nd->left != NULL && nd->right != NULL)
			nd->deleted_cnt = max((nd->left)->deleted_cnt, (nd->right)->deleted_cnt) + nd->is_deleted;
		else if(nd->left != NULL && nd->right == NULL)
			nd->deleted_cnt = (nd->left)->deleted_cnt + nd->is_deleted;
		else if(nd->left == NULL && nd->right != NULL)
			nd->deleted_cnt = (nd->right)->deleted_cnt + nd->is_deleted;

		if(abs(nd->d_left - nd->d_right) > 1){
			int rotation = compute_rotation(nd);

			if(rotation == 1){ // R
				nd = r_rotation(nd);
				R++;
			}
			else if(rotation == 2){ // L
				nd = l_rotation(nd);
				R++;
			}
			else if(rotation == 3){ // LR
				l_rotation(nd->left);
				nd = r_rotation(nd);
				R+=2;
			}
			else if(rotation == 4){ // RL
				r_rotation(nd->right);
				nd = l_rotation(nd);
				R+=2;
			}
			if(nd->parent == NULL)
				ret = nd;
		}

		nd = nd->parent;
	}
	return ret;
}

int compute_rotation(node_t *nd){
	int ret = 0;
	if((nd->d_left - nd->d_right) >= 2){
		if(((nd->left)->d_left - (nd->left)->d_right) == -1)
			ret = 3; // LR
		else
			ret = 1; // R
	}
	else if((nd->d_left - nd->d_right) <= -2){
		if(((nd->right)->d_left - (nd->right)->d_right) == 1)
			ret = 4; // RL
		else
			ret = 2; // L
	}

	return ret;
}

node_t *r_rotation(node_t *node){
	node_t *parent = node->parent;
	node_t *new_subroot = node->left;

	// rebuild subtree
	new_subroot->parent = parent;
	if(parent != NULL){
		if(parent->value < new_subroot->value)
			parent->right = new_subroot;
		else
			parent->left = new_subroot;
	}
	else
		root = new_subroot;
	node->left = new_subroot->right;
	if(new_subroot->right != NULL){
		(new_subroot->right)->parent = node;
	}
	node->parent = new_subroot;
	new_subroot->right = node;

	// update depthes of the nodes
	if(node->left != NULL)
		node->d_left = max((node->left)->d_left, (node->left)->d_right) + 1;
	else
		node->d_left = 0;
	
	new_subroot->d_right = max(node->d_left, node->d_right) + 1;

	// update amount of deleted nodes on the way through the nodes
	if(node->left != NULL && node->right != NULL)
		node->deleted_cnt = max((node->left)->deleted_cnt, (node->right)->deleted_cnt);
	else if(node->left != NULL)
		node->deleted_cnt = (node->left)->deleted_cnt;
	else if(node->right != NULL)
		node->deleted_cnt = (node->right)->deleted_cnt;
	else
		node->deleted_cnt = 0;
	node->deleted_cnt += node->is_deleted;
	
	if(new_subroot->left != NULL && new_subroot->right != NULL)
		new_subroot->deleted_cnt = max((new_subroot->left)->deleted_cnt, (new_subroot->right)->deleted_cnt);
	else if(new_subroot->right != NULL)
		new_subroot->deleted_cnt = (new_subroot->right)->deleted_cnt;
	else if(new_subroot->left != NULL)
		new_subroot->deleted_cnt = (new_subroot->left)->deleted_cnt;
	else
		new_subroot->deleted_cnt = 0;
	new_subroot->deleted_cnt += new_subroot->is_deleted;

	return new_subroot;
}

node_t *l_rotation(node_t *node){
	node_t *parent = node->parent;
	node_t *new_subroot = node->right;

	// rebuild subtree
	new_subroot->parent = parent;
	if(parent != NULL){
		if(parent->value < new_subroot->value)
			parent->right = new_subroot;
		else
			parent->left = new_subroot;
	}
	else
		root = new_subroot;
	node->right = new_subroot->left;
	if(new_subroot->left != NULL){
		(new_subroot->left)->parent = node;
	}
	node->parent = new_subroot;
	new_subroot->left = node;

	// update depthes of the nodes
	if(node->right != NULL)
		node->d_right = max((node->right)->d_left, (node->right)->d_right) + 1;
	else
		node->d_right = 0;
	
	new_subroot->d_left = max(node->d_left, node->d_right) + 1;

	// update amount of deleted nodes on the way through the nodes
	if(node->left != NULL && node->right != NULL)
		node->deleted_cnt = max((node->right)->deleted_cnt, (node->left)->deleted_cnt);
	else if(node->right != NULL)
		node->deleted_cnt = (node->right)->deleted_cnt;
	else if(node->left != NULL)
		node->deleted_cnt = (node->left)->deleted_cnt;
	else
		node->deleted_cnt = 0;
	node->deleted_cnt += node->is_deleted;

	if(new_subroot->left != NULL && new_subroot->right != NULL)
		new_subroot->deleted_cnt = max((new_subroot->left)->deleted_cnt, (new_subroot->right)->deleted_cnt);
	else if(new_subroot->left != NULL)
		new_subroot->deleted_cnt = (new_subroot->left)->deleted_cnt;
	else if(new_subroot->right != NULL)
		new_subroot->deleted_cnt = (new_subroot->right)->deleted_cnt;
	else
		new_subroot->deleted_cnt = 0;
	new_subroot->deleted_cnt += new_subroot->is_deleted;
	
	return new_subroot;
}

void print_tree(node_t *nd){
	if(nd == NULL){
		return;
	}

	if(nd->parent == NULL){
		printf("NULL -> ");
	}
	else{
		printf("%d(%d) -> ", (nd->parent)->value, (nd->parent)->deleted_cnt);
	}
	
	printf("%d(%d) -> ", nd->value, nd->deleted_cnt);
	if(nd->left != NULL)
		printf("%d(%d)", (nd->left)->value, (nd->left)->deleted_cnt);
	else
		printf("null");
	printf(", ");
	if(nd->right != NULL)
		printf("%d(%d)", (nd->right)->value, (nd->right)->deleted_cnt);
	else
		printf("null");
	printf("\n");

	if(nd->left != NULL){
		print_tree(nd->left);
	}
	if(nd->right != NULL){
		print_tree(nd->right);
	}
}

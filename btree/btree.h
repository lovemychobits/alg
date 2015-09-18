#ifndef BTREE_BTREE_H
#define BTREE_BTREE_H

#define NULL 0
#include <algorithm>

// btree节点
struct b_node {
	int num;				// 当前节点key的数量
	int dim;
	int pos_in_parent;		// 在父节点中的位置

	int* keys;
	b_node* parent;			// 父节点
	b_node** childs;		// 所有子节点

	b_node() {
	}

	b_node (int _dim) : num(0), parent(NULL), pos_in_parent(0) {
		dim = _dim;
		keys = new int[dim + 1];				// 预留一个位置，方便处理节点满了的时候插入操作
		childs = new b_node*[dim + 2];			// 扇出肯定需要比key还多一个
		for (int i=0; i<dim+1; ++i) {
			keys[i] = 0;
			childs[i] = NULL;
		}
		childs[dim+1] = NULL;
	}

	// 插入key，返回插入的位置
	int insert(int key) {
		int i = 0;
		keys[num] = key;
		for (i = num; i > 0; --i)
		{
			if (keys[i-1] > keys[i])
			{
				std::swap(keys[i-1], keys[i]);
				continue;
			}
			break;
		}
		++num;							// 数量添加
		return i;
	}

	// 删除指定key
	void del(int key) {
		for (int i=0; i<num; ++i)
		{
			// 找到了节点, 后续所有节点前移
			if (keys[i] == key)
			{
				keys[i] = 0;
				int n = 0;
				for (n=i; n<num-1; ++n)
				{
					std::swap(keys[n], keys[n+1]);
					if (childs[n+1]) {
						childs[n+1]->pos_in_parent -= 1;			// 调整子节点在父节点中的位置信息
						childs[n] = childs[n+1];
					}
				}
				if (childs[n]) {
					childs[n+1]->pos_in_parent -= 1;			// 调整子节点在父节点中的位置信息
					childs[n] = childs[n+1];
				}
				break;
			}
		}
		--num;							// 减少key的数量
		return;
	}

	// 判断是否为根节点
	bool is_root() {
		if (!parent)
		{
			return true;
		}
		return false;
	}

	// 判断是否为叶子节点
	bool is_leaf() { 
		if (!childs[0]) {				// 没有子节点
			return true;
		}
		return false;
	}

	bool is_full() {
		if (num < dim) {
			return false;
		}
		return true;
	}

	// 判断是否underflowed
	bool is_underflowed() {
		if (num < dim/2) {
			return true;
		}
		return false;
	}

	// 获取需要插入的位置
	int get_ins_pos(int key) {
		int i = 0;
		for (i=0; i<dim; ++i) {
			if (key > keys[i] && keys[i]) {
				continue;
			}
		}

		return i;
	}

	// 获取key数量较多的邻居
	b_node* get_pop_ngb() {
		if (!parent)						// 是根节点
		{
			return NULL;
		}
		if (pos_in_parent == 0)				// 父节点中最左边的节点
		{
			return parent->childs[pos_in_parent+1];
		}
		if (pos_in_parent == parent->num)	// 父节点中最右边节点	
		{
			return parent->childs[pos_in_parent-1];
		}

		if (parent->childs[pos_in_parent+1]->num > parent->childs[pos_in_parent-1]->num)
		{
			return parent->childs[pos_in_parent+1];
		}
		else
		{
			return parent->childs[pos_in_parent-1];
		}
		return NULL;
	}
};

// 表达某个值的位置
struct pos {
	b_node* node;			// 所在位置的node指针
	int index;				// 所在node节点的索引
	pos() : node(NULL), index(-1) {
	}
};

class btree {
public:
	btree (int _dim) : dim(_dim), root(NULL) {
	}

	pos query(int key);			// 查找某个某个key
	void insert(int key);		// 插入某个key
	void del(int key);			// 删除某个key
	void print();				// 分层打印btree

private:
	pos _query(b_node* node, int key);
	pos _get_left_max_key(int key);			// 找到某个节点最大左子树的key

	void _print(b_node* node, int level);

	void _insert(b_node* node, int key);
	void _del(b_node* node, int key);
	void _split_node(b_node* node);
	void _link_node(b_node* parent, int pos, b_node* left_child, b_node* right_child);
	void _merge_node(b_node* node);

private:
	int dim;					// 维度
	b_node* root;				// 根节点
};

#endif
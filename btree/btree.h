#ifndef BTREE_BTREE_H
#define BTREE_BTREE_H

#define NULL 0
#include <algorithm>

// btree�ڵ�
struct b_node {
	int num;				// ��ǰ�ڵ�key������
	int dim;
	int pos_in_parent;		// �ڸ��ڵ��е�λ��

	int* keys;
	b_node* parent;			// ���ڵ�
	b_node** childs;		// �����ӽڵ�

	b_node() {
	}

	b_node (int _dim) : num(0), parent(NULL), pos_in_parent(0) {
		dim = _dim;
		keys = new int[dim + 1];				// Ԥ��һ��λ�ã����㴦��ڵ����˵�ʱ��������
		childs = new b_node*[dim + 2];			// �ȳ��϶���Ҫ��key����һ��
		for (int i=0; i<dim+1; ++i) {
			keys[i] = 0;
			childs[i] = NULL;
		}
		childs[dim+1] = NULL;
	}

	// ����key�����ز����λ��
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
		++num;							// �������
		return i;
	}

	// ɾ��ָ��key
	void del(int key) {
		for (int i=0; i<num; ++i)
		{
			// �ҵ��˽ڵ�, �������нڵ�ǰ��
			if (keys[i] == key)
			{
				keys[i] = 0;
				int n = 0;
				for (n=i; n<num-1; ++n)
				{
					std::swap(keys[n], keys[n+1]);
					if (childs[n+1]) {
						childs[n+1]->pos_in_parent -= 1;			// �����ӽڵ��ڸ��ڵ��е�λ����Ϣ
						childs[n] = childs[n+1];
					}
				}
				if (childs[n]) {
					childs[n+1]->pos_in_parent -= 1;			// �����ӽڵ��ڸ��ڵ��е�λ����Ϣ
					childs[n] = childs[n+1];
				}
				break;
			}
		}
		--num;							// ����key������
		return;
	}

	// �ж��Ƿ�Ϊ���ڵ�
	bool is_root() {
		if (!parent)
		{
			return true;
		}
		return false;
	}

	// �ж��Ƿ�ΪҶ�ӽڵ�
	bool is_leaf() { 
		if (!childs[0]) {				// û���ӽڵ�
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

	// �ж��Ƿ�underflowed
	bool is_underflowed() {
		if (num < dim/2) {
			return true;
		}
		return false;
	}

	// ��ȡ��Ҫ�����λ��
	int get_ins_pos(int key) {
		int i = 0;
		for (i=0; i<dim; ++i) {
			if (key > keys[i] && keys[i]) {
				continue;
			}
		}

		return i;
	}

	// ��ȡkey�����϶���ھ�
	b_node* get_pop_ngb() {
		if (!parent)						// �Ǹ��ڵ�
		{
			return NULL;
		}
		if (pos_in_parent == 0)				// ���ڵ�������ߵĽڵ�
		{
			return parent->childs[pos_in_parent+1];
		}
		if (pos_in_parent == parent->num)	// ���ڵ������ұ߽ڵ�	
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

// ���ĳ��ֵ��λ��
struct pos {
	b_node* node;			// ����λ�õ�nodeָ��
	int index;				// ����node�ڵ������
	pos() : node(NULL), index(-1) {
	}
};

class btree {
public:
	btree (int _dim) : dim(_dim), root(NULL) {
	}

	pos query(int key);			// ����ĳ��ĳ��key
	void insert(int key);		// ����ĳ��key
	void del(int key);			// ɾ��ĳ��key
	void print();				// �ֲ��ӡbtree

private:
	pos _query(b_node* node, int key);
	pos _get_left_max_key(int key);			// �ҵ�ĳ���ڵ������������key

	void _print(b_node* node, int level);

	void _insert(b_node* node, int key);
	void _del(b_node* node, int key);
	void _split_node(b_node* node);
	void _link_node(b_node* parent, int pos, b_node* left_child, b_node* right_child);
	void _merge_node(b_node* node);

private:
	int dim;					// ά��
	b_node* root;				// ���ڵ�
};

#endif
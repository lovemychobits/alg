#include "btree.h"
#include <iostream>
using namespace std;

pos btree::query(int key) {
	return _query(root, key);
}

pos btree::_query(b_node* node, int key) {
	pos p;
	if (!node)
	{
		return p;
	}

	int index = 0;				// 在节点中的索引
	while (index < node->num && node->keys[index] < key)
	{
		++index;
	}

	// 如果找到了
	if (node->keys[index] == key)
	{
		// 找到key了
		p.node = node;
		p.index = index;
		return p;
	}

	return _query(node->childs[index], key);
}

pos btree::_get_left_max_key(int key) {
	pos p = _query(root, key);					// 先找到指定节点的具体位置
	pos p2;

	if (p.node->is_leaf())
	{
		return p2;
	}

	// 然后在找其最大左子树的key位置
	b_node* node = p.node;
	while (!node->is_leaf()) {
		node = node->childs[node->num-1];		// 一直找最右边的子树，直到当前节点是叶子节点
	}

	p2.index = node->num-1;
	p2.node = node;
	return p2;
}

void btree::insert(int key) {
	_insert(root, key);
}

void btree::_insert(b_node* node, int key) {

	// 根节点为空
	if (root == NULL)
	{
		root = new b_node(dim);
		root->insert(key);
		return;
	}
	
	int index = node->num;
	while (index > 0 && node->keys[index-1] > key)				// 找到对应的子节点
	{
		--index;
	}
	
	// 如果当前node插入节点已经没有左右儿子了，那么就在当前节点中插入
	if (!node->childs[index])					// 因为btree一定是既有左儿子，又有右儿子，所以只判断其中一个是否存在就可以了
	{
		// 如果节点没有满
		if (!node->is_full())
		{
			node->insert(key);
			return;
		}

		// 如果当前节点已经满了，需要将中间节点拆分，然后加入到父节点中，将剩余的2个部分，作为新节点的左右子节点
		// 如果父节点加入新的key之后也满了，那么递归上一个步骤
		node->insert(key);
		_split_node(node);
		return;
	}

	// 已经遍历到最右key了
	if (index == node->num)
	{
		_insert(node->childs[index], key);
		return;
	}

	_insert(node->childs[index], key);
	return;
}

void btree::_split_node(b_node* node) {
	if (!node || !node->is_full()) {
		return;
	}

	int split_pos = (node->dim-2)/2 + 1;				// 分割点
	int split_value = node->keys[split_pos];
	b_node* split_left_node = new b_node(dim);
	b_node* split_right_node = new b_node(dim);
	
	// 处理左儿子节点
	int i = 0;
	int j = 0;
	for (; i<split_pos; ++i, ++j) {
		split_left_node->keys[i] = node->keys[j];
		split_left_node->childs[i] = node->childs[j];
		
		if (split_left_node->childs[i]) {
			split_left_node->childs[i]->parent = split_left_node;
			split_left_node->childs[i]->pos_in_parent = i;
		}
	}
	split_left_node->childs[i] = node->childs[j];
	if (split_left_node->childs[i]) {
		split_left_node->childs[i]->parent = split_left_node;
		split_left_node->childs[i]->pos_in_parent = i;
	}
	split_left_node->num = split_pos;

	// 处理右儿子节点
	for (i = 0, j=split_pos+1; i < dim - split_pos; ++i, ++j) {
		split_right_node->keys[i] = node->keys[j];
		split_right_node->childs[i] = node->childs[j];
		
		if (split_right_node->childs[i]) {
			split_right_node->childs[i]->parent = split_right_node;
			split_right_node->childs[i]->pos_in_parent = i;
		}
	}
	split_right_node->childs[i] = node->childs[j];
	if (split_right_node->childs[i]) {
		split_right_node->childs[i]->parent = split_right_node;
		split_right_node->childs[i]->pos_in_parent = i;
	}
	split_right_node->num = dim - split_pos;

	// 将分割的节点上升到父节点中
	b_node* parent = node->parent;
	if (!parent) {			// 父节点不存在
		b_node* new_parent = new b_node(dim);
		new_parent->insert(split_value);

		_link_node(new_parent, 0, split_left_node, split_right_node);

		// 重置根节点
		root = new_parent;	
		return;
	}

	// 如果父节点也满了，那么先将split出来的节点加入父节点，然后再对父节点split
	if (parent->is_full()) {
		int new_pos = parent->insert(split_value);

		_link_node(parent, new_pos, split_left_node, split_right_node);
		_split_node(parent);					// 如果父节点也满了， 那么继续split父节点
	}
	else {
		int pos = parent->insert(split_value);
		_link_node(parent, pos, split_left_node, split_right_node);
	}

	return;
}

// 重连节点
void btree::_link_node(b_node* parent, int pos, b_node* left_child, b_node* right_child) {
	parent->childs[pos] = left_child;
	left_child->parent = parent;
	left_child->pos_in_parent = pos;
	
	parent->childs[pos+1] = right_child;
	right_child->parent = parent;
	right_child->pos_in_parent = pos + 1;
}

void btree::del(int key) {
	_del(root, key);
}

void btree::_del(b_node* node, int key) {
	// 先找到删除节点所在的位置
	pos p = query(key);

	// 查找其最大左子树key
	pos left_max_p = _get_left_max_key(key);

	b_node* del_node = p.node;
	if (left_max_p.node != NULL)
	{
		del_node = left_max_p.node;
		std::swap(p.node->keys[p.index], left_max_p.node->keys[left_max_p.index]);	// 将最大左子树key和当前key进行交换
	}

	// 现在针对key进行删除
	del_node->del(key);	

	// 先判断如果没有underflowed，就直接结束了
	if (!del_node->is_underflowed()) {
		return;
	}

	_merge_node(del_node);
}

void btree::_merge_node(b_node* del_node) {
	// 如果underflowed了，那么先判断是否为根节点，根节点只要最少有一个key就可以了，其他非根节点最少要有(M-1)/2个key
	if (del_node->is_root())
	{
		if (del_node->num == 0)				// 根节点已经没有key了
		{
			root = del_node->childs[0];
		}
		return;
	}

	// 如果是叶子节点并且underflowed了，那么就需要从其“邻居”来“借”了
	b_node* ngb_node = del_node->get_pop_ngb();
	if (ngb_node == NULL)
	{
		return;
	}

	int p_key_pos = (del_node->pos_in_parent + ngb_node->pos_in_parent) / 2;
	int parent_key = del_node->parent->keys[p_key_pos];

	// 处理组合后的节点
	b_node* combined_node = new b_node(del_node->num + 1 + ngb_node->num);

	if (del_node->pos_in_parent < ngb_node->pos_in_parent)
	{
		int combined_n = 0;
		_realloc(combined_node, del_node, del_node->num);
		combined_n += del_node->num;

		combined_node->insert(parent_key); ++combined_n;

		_realloc(combined_node, ngb_node, ngb_node->num, combined_n);
	}
	else
	{
		int combined_n = 0;
		_realloc(combined_node, ngb_node, ngb_node->num);
		combined_n += ngb_node->num;

		combined_node->insert(parent_key); ++combined_n;

		_realloc(combined_node, del_node, del_node->num, combined_n);
	}


	// 如果邻居key的数量大于(M-1)/2, 那么执行case1逻辑，将combined后的node中间值和parent中的值进行交换，然后分裂成2个节点
	if (ngb_node->num > dim/2)
	{
		int split_pos = (del_node->num + ngb_node->num + 1) / 2;
		b_node* combined_left = new b_node(dim);
		b_node* combined_right = new b_node(dim);

		_realloc(combined_left, combined_node, split_pos);
		_realloc(combined_right, combined_node, combined_node->num - split_pos - 1, 0, split_pos + 1);

		combined_left->parent = del_node->parent;
		combined_right->parent = del_node->parent;

		b_node* parent = del_node->parent;
		std::swap(combined_node->keys[split_pos], del_node->parent->keys[del_node->pos_in_parent]);
		parent->childs[p_key_pos] = combined_left;
		combined_left->pos_in_parent = p_key_pos;
		parent->childs[p_key_pos + 1] = combined_right;
		combined_right->pos_in_parent = p_key_pos + 1;
		
		return;
	}

	// 如果邻居的key的数量刚好是(M-1)/2，那么合并之后就可能会发生underflowed情况
	// 邻居key的数量不可能会发生小于(M-1)/2的，因为如果是这样，之前就已经做过fix处理了
	del_node->parent->del(parent_key);
	del_node->parent->childs[del_node->pos_in_parent] = combined_node;
	combined_node->parent = del_node->parent;
	combined_node->pos_in_parent = del_node->pos_in_parent;

	// 如果parent去掉一个节点之后并没有underflowed，那么就结束
	if (!del_node->parent->is_underflowed())
	{
		return;
	}

	// 否则继续对parent节点进行修复, 直到根节点
	_merge_node(del_node->parent);
	return;
}

void btree::_realloc(b_node* new_node, b_node* old_node, int num, int new_offset, int old_offset) {
	int i = old_offset;
	int n = new_offset;
	for (; i<old_offset + num; ++i, ++n)
	{
		new_node->keys[n] = old_node->keys[i];
		new_node->childs[n] = old_node->childs[i];

		if (new_node->childs[n]) {
			new_node->childs[n]->parent = new_node;
			new_node->childs[n]->pos_in_parent = n;
		}
	}
	new_node->childs[n] = old_node->childs[i];
	if (new_node->childs[n]) {
		new_node->childs[n]->parent = new_node;
		new_node->childs[n]->pos_in_parent = n;
	}
	new_node->num += num;
	return;
}

void btree::print() {
	cout << "==================================" << endl;
	_print(root, 1);
	cout << "==================================" << endl;
}

void btree::_print(b_node* node, int level) {
	if (!node) {
		return;
	}

	cout << "level=" << level << "," << "num=" << node->num << ", in_parent_pos=" << node->pos_in_parent << "==>\t";
	for (int i=0; i<node->num; ++i)	{
		cout << node->keys[i] << ",";
	}
	cout << endl;

	for (int i=0; i<node->num+1; ++i) {
		_print(node->childs[i], level+1);
	}
	return;
}
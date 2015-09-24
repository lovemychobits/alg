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

	int index = 0;				// �ڽڵ��е�����
	while (index < node->num && node->keys[index] < key)
	{
		++index;
	}

	// ����ҵ���
	if (node->keys[index] == key)
	{
		// �ҵ�key��
		p.node = node;
		p.index = index;
		return p;
	}

	return _query(node->childs[index], key);
}

pos btree::_get_left_max_key(int key) {
	pos p = _query(root, key);					// ���ҵ�ָ���ڵ�ľ���λ��
	pos p2;

	if (p.node->is_leaf())
	{
		return p2;
	}

	// Ȼ�������������������keyλ��
	b_node* node = p.node;
	while (!node->is_leaf()) {
		node = node->childs[node->num-1];		// һֱ�����ұߵ�������ֱ����ǰ�ڵ���Ҷ�ӽڵ�
	}

	p2.index = node->num-1;
	p2.node = node;
	return p2;
}

void btree::insert(int key) {
	_insert(root, key);
}

void btree::_insert(b_node* node, int key) {

	// ���ڵ�Ϊ��
	if (root == NULL)
	{
		root = new b_node(dim);
		root->insert(key);
		return;
	}
	
	int index = node->num;
	while (index > 0 && node->keys[index-1] > key)				// �ҵ���Ӧ���ӽڵ�
	{
		--index;
	}
	
	// �����ǰnode����ڵ��Ѿ�û�����Ҷ����ˣ���ô���ڵ�ǰ�ڵ��в���
	if (!node->childs[index])					// ��Ϊbtreeһ���Ǽ�������ӣ������Ҷ��ӣ�����ֻ�ж�����һ���Ƿ���ھͿ�����
	{
		// ����ڵ�û����
		if (!node->is_full())
		{
			node->insert(key);
			return;
		}

		// �����ǰ�ڵ��Ѿ����ˣ���Ҫ���м�ڵ��֣�Ȼ����뵽���ڵ��У���ʣ���2�����֣���Ϊ�½ڵ�������ӽڵ�
		// ������ڵ�����µ�key֮��Ҳ���ˣ���ô�ݹ���һ������
		node->insert(key);
		_split_node(node);
		return;
	}

	// �Ѿ�����������key��
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

	int split_pos = (node->dim-2)/2 + 1;				// �ָ��
	int split_value = node->keys[split_pos];
	b_node* split_left_node = new b_node(dim);
	b_node* split_right_node = new b_node(dim);
	
	// ��������ӽڵ�
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

	// �����Ҷ��ӽڵ�
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

	// ���ָ�Ľڵ����������ڵ���
	b_node* parent = node->parent;
	if (!parent) {			// ���ڵ㲻����
		b_node* new_parent = new b_node(dim);
		new_parent->insert(split_value);

		_link_node(new_parent, 0, split_left_node, split_right_node);

		// ���ø��ڵ�
		root = new_parent;	
		return;
	}

	// ������ڵ�Ҳ���ˣ���ô�Ƚ�split�����Ľڵ���븸�ڵ㣬Ȼ���ٶԸ��ڵ�split
	if (parent->is_full()) {
		int new_pos = parent->insert(split_value);

		_link_node(parent, new_pos, split_left_node, split_right_node);
		_split_node(parent);					// ������ڵ�Ҳ���ˣ� ��ô����split���ڵ�
	}
	else {
		int pos = parent->insert(split_value);
		_link_node(parent, pos, split_left_node, split_right_node);
	}

	return;
}

// �����ڵ�
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
	// ���ҵ�ɾ���ڵ����ڵ�λ��
	pos p = query(key);

	// ���������������key
	pos left_max_p = _get_left_max_key(key);

	b_node* del_node = p.node;
	if (left_max_p.node != NULL)
	{
		del_node = left_max_p.node;
		std::swap(p.node->keys[p.index], left_max_p.node->keys[left_max_p.index]);	// �����������key�͵�ǰkey���н���
	}

	// �������key����ɾ��
	del_node->del(key);	

	// ���ж����û��underflowed����ֱ�ӽ�����
	if (!del_node->is_underflowed()) {
		return;
	}

	_merge_node(del_node);
}

void btree::_merge_node(b_node* del_node) {
	// ���underflowed�ˣ���ô���ж��Ƿ�Ϊ���ڵ㣬���ڵ�ֻҪ������һ��key�Ϳ����ˣ������Ǹ��ڵ�����Ҫ��(M-1)/2��key
	if (del_node->is_root())
	{
		if (del_node->num == 0)				// ���ڵ��Ѿ�û��key��
		{
			root = del_node->childs[0];
		}
		return;
	}

	// �����Ҷ�ӽڵ㲢��underflowed�ˣ���ô����Ҫ���䡰�ھӡ������衱��
	b_node* ngb_node = del_node->get_pop_ngb();
	if (ngb_node == NULL)
	{
		return;
	}

	int p_key_pos = (del_node->pos_in_parent + ngb_node->pos_in_parent) / 2;
	int parent_key = del_node->parent->keys[p_key_pos];

	// ������Ϻ�Ľڵ�
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


	// ����ھ�key����������(M-1)/2, ��ôִ��case1�߼�����combined���node�м�ֵ��parent�е�ֵ���н�����Ȼ����ѳ�2���ڵ�
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

	// ����ھӵ�key�������պ���(M-1)/2����ô�ϲ�֮��Ϳ��ܻᷢ��underflowed���
	// �ھ�key�����������ܻᷢ��С��(M-1)/2�ģ���Ϊ�����������֮ǰ���Ѿ�����fix������
	del_node->parent->del(parent_key);
	del_node->parent->childs[del_node->pos_in_parent] = combined_node;
	combined_node->parent = del_node->parent;
	combined_node->pos_in_parent = del_node->pos_in_parent;

	// ���parentȥ��һ���ڵ�֮��û��underflowed����ô�ͽ���
	if (!del_node->parent->is_underflowed())
	{
		return;
	}

	// ���������parent�ڵ�����޸�, ֱ�����ڵ�
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
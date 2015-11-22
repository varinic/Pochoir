/*
 * ============================================================================
 *       Filename:  projections.hpp
 *    Description:  Has routines 
 *					1. that count the # of projections in
 *					the default space/time cut scheme and the
 *					modified space/power of two time cut scheme.
 *					2. that implement the modified space/power of two
 *					time cuts.
 *					The code uses the time/space cut code framework in
 *					pochoir_walk_recursive.hpp
 *        Created:  03/25/2013
 *         Author:  Eka Palamadai, epn@mit.edu
 * ============================================================================
 */
#ifndef HETEROGENEITY_HPP
#define HETEROGENEITY_HPP

#include "rbq_common.h"
#include <deque>
#include <unordered_map>
#include <climits> 
using namespace std ;

typedef unsigned int word_type ;
/*template <int N_RANK>
class zoid_structure
{
	int height ;
	zoid_strcuture * children ;
	int width [N_RANK] ;
	int converging [N_RANK] ;
} ;*/

template <int N_RANK>
class zoid
{
	public :

	void resize_children(int size)
	{
		//cout << "resize child for zoid " << id << " # children " << size << endl ;
		assert (size) ;
		//children.resize(size, 0) ;	
		children = new unsigned long [size];
		num_children = size ;
		for (int i = 0 ; i < size ; i++)
		{
			children [i] = 0 ;
		}
		//cout << "resize done " << endl ;
	}

	//void add_child(zoid * child, int child_index)
	//void add_child(zoid * child, int child_index, unsigned long global_index)
	void add_child(zoid * child, int pos, unsigned long index)
	{
		//cout << "adding child for zoid " << id << endl ; 
		assert (num_children) ;
		assert (pos < num_children) ;
		assert (child) ;
		
		//don't add the zoid as its own child
		if (this != child)
		{
			//child->add_parent() ;
			//children.push_back(child) ;
			children [pos] = index ;
#ifndef NDEBUG
			/*if (N_RANK == 1)
			{
				assert (pos < 3) ;
			}*/
			child->add_parent(this->id) ;
#endif
		}
	}
	
	zoid() 
	{
		//cout << "zoid : constructor " << endl ;
		geneity = 0 ;
		children = 0 ;
		num_children = 0 ;
		height = 0 ;
#ifndef NDEBUG
		id = ULONG_MAX ;
#endif
	};
	
	zoid & operator = (const zoid & z)
	{
		//cout << "zoid : assignment op for zoid " << z.id << endl ;
		if (this != &z)
		{
			delete [] children ;
			geneity = z.geneity ;
			num_children = z.num_children ;
			children = 0 ;
			height = z.height ;
			if (num_children > 0)
			{
				children = new unsigned long [num_children];
				for (int i = 0 ; i < num_children ; i++)
				{
					children [i] = z.children [i] ;
				}
			}
#ifndef NDEBUG
			id = z.id ;
			info = z.info ;
			for (int i = 0 ; i < z.parents.size() ; i++)
			{
				parents.push_back(z.parents [i]) ;
			}
#endif
		}
		return *this ;
	}
	
	zoid(const zoid & z)
	{
		geneity = z.geneity ;
		num_children = z.num_children ;
		//cout << "zoid : copy const for zoid " << z.id << " # children" << 
		//		num_children << endl ;
		children = 0 ;
		height = z.height ;
		if (num_children > 0)
		{
			children = new unsigned long [num_children];
			for (int i = 0 ; i < num_children ; i++)
			{
				children [i] = z.children [i] ;
			}
		}
#ifndef NDEBUG
		id = z.id ;
		info = z.info ;
		for (int i = 0 ; i < z.parents.size() ; i++)
		{
			parents.push_back(z.parents [i]) ;
		}
#endif
	}

	/*void add_parent()
	{
		num_parents++ ;
	}

	void remove_parent()
	{
		num_parents-- ;
	}

	int get_parent_count()
	{
		return num_parents ;
	}*/

	int add_parent(unsigned long parent_id)
	{
#ifndef NDEBUG
		parents.push_back(parent_id) ;
#endif
	}
	//destructor for zoid
	//need to delete children as well.
	//but same child may be referred by more than one parent.
	//can't delete a child pointer more than once
	~zoid()
	{
		//cout << "zoid : destructor for zoid " << id << endl ;
		num_children = 0 ;
		delete [] children ;
		children = 0 ;
		/*assert (num_parents == 0) ;
#ifndef NDEBUG
		if (num_parents > 0)
		{
			cout << "zoid : error -- num_parents > 0 " << endl ;
		}
#endif
		for (int i = 0 ; i < children.size() ; i++)
		{
			if (children [i])
			{
				children [i]->remove_parent() ;
				//if child has no more parents, delete it.
				if (children [i]->get_parent_count() == 0)
				{
					delete children [i] ;
				}
			}
		}*/	
	}
	
	private :
	word_type geneity ;
	int height ;
	//int num_parents ; //# of parents for the zoid
	//zoid_structure * structure ;
	unsigned long * children ;  
	int num_children ;
#ifndef NDEBUG
	grid_info <N_RANK> info ;
	unsigned long id ; //id of the zoid.
	vector<unsigned long> parents ;
#endif
} ;

// a compact representation of zoid
class simple_zoid
{
	word_type geneity ;
	int child_start_index ;
	int child_end_index ;
} ;

/*
template<>
zoid<1>::zoid()
{
	//num_parents = 0 ;
	geneity = 0 ;
	//children.reserve(2) ;
	//children.reserve(3) ;
	//children.resize(3,0) ;
}

template<>
zoid<2>::zoid()
{
	//num_parents = 0 ;
	geneity = 0 ;
	//children.reserve(2) ;
	//children.reserve(9) ;
	//children.resize(9,0) ;
}*/
/*
template <>
class zoid_sort_criterion
{
	public :
	bool operator() (const zoid<1> & z1, const zoid<1> & z2) const
	{
		grid_info <N_RANK> const & g1 = z1.info ;
		grid_info <N_RANK> const & g2 = z2.info ;
		int x0 = g1.x0 [0] ;
		int x1 = g1.x1 [0] ; 
		int x2 = g1.x0 [0] + g1.dx0 [0] * (dt - 1) ;
	}
} ; */

template <int N_RANK>
class heterogeneity
{
private:
	typedef zoid <N_RANK> zoid_type ;
	//typedef multimap<unsigned long, zoid_type *> mmap ;
	//typedef typename multimap<unsigned long, zoid_type *>::iterator mmap_iterator ;
	//typedef unordered_multimap<unsigned long, zoid_type *> hash_table ;
	typedef unordered_multimap<unsigned long, unsigned long> hash_table ;
	//typedef typename unordered_multimap<unsigned long, zoid_type *>::iterator 
	typedef typename unordered_multimap<unsigned long, unsigned long>::iterator 
					hash_table_iterator ;

	void initialize(grid_info<N_RANK> const & grid, bool power_of_two)
	{
		unsigned long volume = 1 ;
		for (int i = 0 ; i < N_RANK ; i++)
		{
			volume *= (grid.x1[i] - grid.x0[i]) ;		
		}
		m_projections.reserve(volume) ;
		m_projections.resize(volume) ;
#ifdef COUNT_PROJECTIONS
		if (N_RANK == 1)
		{
			m_1d_count_proj_length_triangle.reserve(volume + 1) ;
			m_1d_count_proj_length_triangle.resize(volume + 1) ;
			m_1d_count_proj_length_trapezoid.reserve(volume + 1) ;
			m_1d_count_proj_length_trapezoid.resize(volume + 1) ;

			m_1d_index_by_length_triangle.reserve(volume + 1) ;
			m_1d_index_by_length_triangle.resize(volume + 1) ;
			m_1d_index_by_length_trapezoid.reserve(volume + 1) ;
			m_1d_index_by_length_trapezoid.resize(volume + 1) ;

			m_1d_count_proj_centroid.reserve(volume + 1) ; 
			m_1d_count_proj_centroid.resize(volume + 1) ; 
			
			m_1d_tallest_zoid_per_centroid.reserve(volume + 1) ; 
			m_1d_tallest_zoid_per_centroid.resize(volume + 1) ; 

			m_num_triangles = 0 ;
			m_num_trapezoids = 0 ;
		}
#endif
		//unsigned long P = volume / (1 << N_RANK) ; 
		//int lgP = 8 * sizeof(unsigned long) - __builtin_clzl(P - 1) ;
		//cout << "Expected # of projections P " << P  << " lg P " << lgP << endl ;
		if (power_of_two)
		{
			m_zoids.reserve(volume / 16) ;
			cout << "Expected # of projections P " << volume / 16 << endl ;
		}
		else
		{
			m_zoids.reserve(volume / 8) ;
			cout << "Expected # of projections P " << volume / 8 << endl ;
		}
	}

	template <typename F>
	inline void build_heterogeneity_dag(int t0, int t1, 
						grid_info<N_RANK> const & grid, F const & f, int index)
	{
		//assert (m_head [index] == (zoid_type *) 0) ;
		assert (m_head [index] == ULONG_MAX) ;
		assert (m_projections.size()) ;
		//create a dummy head
		m_zoids.push_back(zoid_type ()) ;
		zoid_type & dummy_head = m_zoids [m_num_vertices] ;
#ifndef NDEBUG
		dummy_head.id = m_num_vertices ;
#endif
		dummy_head.resize_children(1) ;
		m_num_vertices++ ;
		assert (m_num_vertices == m_zoids.size()) ;
		m_head [index] = m_num_vertices ;
		//assert (m_num_vertices == m_num_projections) ;
		cout << "t0 " << t0 << " t1 " << t1 << endl ;
		symbolic_space_time_cut_boundary(t0, t1, grid, m_num_vertices - 1, 0, f) ;
		//symbolic_space_time_cut_boundary(t0, t1, grid, &dummy_head, 0, f) ;
		//the first child of dummy_head is the actual head
		//m_head [index] = dummy_head.children [0] ; 
		//m_head [index]->remove_parent() ; //remove the dummy parent.
		//m_head->id = m_num_vertices++ ;
		//remove m_head [index] from children of dummy
		//dummy_head.children [0] = 0 ; 
		//vector<zoid_type *> ().swap(dummy_head.children) ;	
		//cout << "# of parents of head " << m_head [index]->get_parent_count() 
		//	<< endl ;
	} 

	template <typename F>
	inline void build_heterogeneity_dag_modified(int t0, int t1, 
						grid_info<N_RANK> const & grid, F const & f, int index)
	{
		//assert (m_head [index] == (zoid_type *) 0) ;
		assert (m_head [index] == ULONG_MAX) ;
		assert (m_projections.size()) ;
		//create a dummy head
		m_zoids.push_back(zoid_type ()) ;
		zoid_type & dummy_head = m_zoids [m_num_vertices] ;
#ifndef NDEBUG
		dummy_head.id = m_num_vertices ;
#endif
		dummy_head.resize_children(1) ;
		m_num_vertices++ ;
		assert (m_num_vertices == m_zoids.size()) ;
		m_head [index] = m_num_vertices ;
		//assert (m_num_vertices == m_num_projections) ;
		cout << "t0 " << t0 << " t1 " << t1 << endl ;
		symbolic_modified_space_time_cut_boundary(t0, t1, grid, 
										m_num_vertices - 1, 0, f) ;
		//symbolic_space_time_cut_boundary(t0, t1, grid, &dummy_head, 0, f) ;
		//the first child of dummy_head is the actual head
		//m_head [index] = dummy_head.children [0] ; 
		//m_head [index]->remove_parent() ; //remove the dummy parent.
		//m_head->id = m_num_vertices++ ;
		//remove m_head [index] from children of dummy
		//dummy_head.children [0] = 0 ; 
		//vector<zoid_type *> ().swap(dummy_head.children) ;	
		//cout << "# of parents of head " << m_head [index]->get_parent_count() 
		//	<< endl ;
	}

	template <typename F>
	void compute_geneity(int h, grid_info<N_RANK> const & grid, 
						word_type & geneity, F const & f) ;

	inline void clear_projections()
	{
		for (int i = 0 ; i < m_projections.size() ; i++)
		{
			m_projections [i].clear() ;		//clear the projections
		}
		m_projections.clear() ;
		//empty the projections vector.
		vector<hash_table>().swap(m_projections) ; 
	}

	inline void print_statistics(int T)
	{
		cout << " Triangles " << endl ;
		cout << "length , # of diff projs of length " << endl ;
		int num_triangle_lengths = 0 ;
		int num_trap_lengths = 0 ;
		for (int i = 0 ; i < m_1d_count_proj_length_triangle.size() ; i++)
		{
			if (m_1d_count_proj_length_triangle [i] >= T)
			{
				num_triangle_lengths++ ; 
				cout << i << " : " << m_1d_count_proj_length_triangle [i] << endl ;
			}
		}
		cout << " # of triangle lengths > " << T << " : " << num_triangle_lengths << endl ;
		cout << endl << endl << " Trapezoids " << endl ;
		cout << "length , # of diff projs of length " << endl ;
		for (int i = 0 ; i < m_1d_count_proj_length_trapezoid.size() ; i++)
		{
			if (m_1d_count_proj_length_trapezoid [i] >= T)
			{
				num_trap_lengths++ ;
				cout << i << " : " << m_1d_count_proj_length_trapezoid [i] << endl ;
			}
		}
		cout << " # of trapezoid lengths > " << T << " : " << num_trap_lengths << endl ;
		int count = 0, centroid = -1 ;
		for (int i = 0 ; i < m_1d_count_proj_centroid.size() ; i++)
		{
			if (m_1d_count_proj_centroid [i] > count)
			{
				count = m_1d_count_proj_centroid [i] ;
				centroid = i ;
			}
		}
		cout << " max # of projections : " << count << " at centroid : " <<
				centroid << endl ; 
		cout << " log 2 (" << T << ") : " << log2(T) << endl ;
	}
	/*{
		cout << " Triangles " << endl ;
		cout << "length , # of diff projs of length " << endl ;
		for (int i = 0 ; i < m_1d_count_proj_length_triangle.size() ; i++)
		{
			if (m_1d_count_proj_length_triangle [i] > 0)
			{
				cout << i << " : " << m_1d_count_proj_length_triangle [i] << endl ;
			}
		}
		for (int i = 0 ;  i < m_1d_index_by_length_triangle.size() ; i++)
		{
			set<unsigned long> & s = m_1d_index_by_length_triangle [i] ;
			if (s.size() > 0)
			{
				cout << endl << " length " << i
				<< " # of cells " << s.size() << endl ;
			}
			for (set<unsigned long> ::iterator it = s.begin() ; it != s.end() ;
													++it)
			{
				cout << *it << "," ;
			}
		}

		cout << endl << endl << " Trapezoids " << endl ;
		cout << "length , # of diff projs of length " << endl ;
		for (int i = 0 ; i < m_1d_count_proj_length_trapezoid.size() ; i++)
		{
			if (m_1d_count_proj_length_trapezoid [i] > 0)
			{
				cout << i << " : " << m_1d_count_proj_length_trapezoid [i] << endl ;
			}
		}
		for (int i = 0 ;  i < m_1d_index_by_length_trapezoid.size() ; i++)
		{
			set<unsigned long> & s = m_1d_index_by_length_trapezoid [i] ;
			if (s.size() > 0)
			{
				cout << endl << " length " << i
				<< " # of cells " << s.size() << endl ;
			}
			for (set<unsigned long> ::iterator it = s.begin() ; it != s.end() ;
													++it)
			{
				cout << *it << "," ;
			}
		}
	
	}*/

	inline void destroy_heterogeneity_dag()
	{
		//delete m_head [0] ;
		//delete m_head [1] ;
		//m_head [0] = 0 ;
		//m_head [1] = 0 ;
		m_head.clear() ;
		m_num_vertices = 0 ;
		//m_num_projections = 0 ;
		clear_projections() ;
		m_heterogeneity.clear() ;
		m_zoids.clear() ;
		vector<zoid_type>().swap(m_zoids) ; //empty the zoids vector
	}
	
	//key is the bottom volume + top volume.
	inline bool check_and_create_projection (unsigned long key, 
					int height, int centroid, unsigned long & index, 
					//int height, int centroid, zoid_type ** zoid, 
					grid_info <N_RANK> const & grid)
	{
		assert (m_projections.size()) ;
		hash_table & h = m_projections [centroid] ;
		//cout << "searching hashtable" << endl ;
		//cout << "size hashtable " << h.size() << endl ;
		std::pair<hash_table_iterator, hash_table_iterator> p = 
													h.equal_range (key) ;
		
		//hash_table iterator has two elements, first and second.
		for (hash_table_iterator start = p.first ; start != p.second ; start++)
		{
			assert (start->first == key) ;
			assert (start->second < m_num_vertices) ;
			zoid_type * z = &(m_zoids [start->second]) ;
			//zoid_type * z = start->second ;
			//assert (z->height == height) ;
			if (z->height == height) 
			{
				//*zoid = z ;
				//cout << "found entry" << endl ;
				index = start->second ;
				grid_info <N_RANK> grid2 = z->info ;
				int h = height ;
				/*bool found = true ;
				for (int i = N_RANK - 1 ; i >= 0 ; i--)
				{
					int x2 = grid.x0 [i] + grid.dx0 [i] * h ;
					int x3 = grid.x1 [i] + grid.dx1 [i] * h ;
					int x2_ = grid2.x0 [i] + grid2.dx0 [i] * h ; 
					int x3_ = grid2.x1 [i] + grid2.dx1 [i] * h ;
					int l = m_algo.phys_length_ [i] ;
					if (pmod(grid.x0 [i], l) != pmod(grid2.x0 [i], l) ||
						pmod(grid.x1 [i], l) != pmod(grid2.x1 [i], l) ||
						pmod(x2, l) != pmod(x2_, l) ||
						pmod(x3, l) != pmod(x3_, l))
					{
						found = false ;
					}
				}
				if (found)
				{
					index = start->second ;
					return true ;
				}*/
#ifndef NDEBUG
//#if 0
				for (int i = N_RANK - 1 ; i >= 0 ; i--)
				{
					int x2 = grid.x0 [i] + grid.dx0 [i] * h ;
					int x3 = grid.x1 [i] + grid.dx1 [i] * h ;
					int x2_ = grid2.x0 [i] + grid2.dx0 [i] * h ; 
					int x3_ = grid2.x1 [i] + grid2.dx1 [i] * h ;
					if (pmod(grid.x0 [i], m_algo.phys_length_ [i]) != 
						pmod(grid2.x0 [i], m_algo.phys_length_ [i]) ||
						pmod(grid.x1 [i], m_algo.phys_length_ [i]) != 
						pmod(grid2.x1 [i], m_algo.phys_length_ [i]) ||
						pmod(x2, m_algo.phys_length_ [i]) != 
						pmod(x2_, m_algo.phys_length_ [i]) ||
						pmod(x3, m_algo.phys_length_ [i]) != 
						pmod(x3_, m_algo.phys_length_ [i]))
					{
						cout << "2 diff zoids hash to same key " << endl ;
						cout << "diff dim " << i << endl ;
						cout << "centroid " << centroid << endl ;
						cout << " grid " << endl ;
						for (int j = N_RANK - 1 ; j >= 0 ; j--)
						{
							cout << " x0 [" << j << "] " << grid.x0 [j] 
							<< " x1 [" << j << "] " << grid.x1 [j] 
							<< " x2 [" << j << "] " << grid.x0[j] + grid.dx0[j] * h
							<< " x3 [" << j << "] " << grid.x1[j] + grid.dx1[j] * h
							<< " h " << h << endl ; 
						}
						cout << " grid 2 at index " << index << endl ;
						for (int j = N_RANK - 1 ; j >= 0 ; j--)
						{
							cout << " x0 [" << j << "] " << grid2.x0 [j] 
							<< " x1 [" << j << "] " << grid2.x1 [j] 
							<< " x2 [" << j << "] " << grid2.x0[j] + grid2.dx0[j] * h
							<< " x3 [" << j << "] " << grid2.x1[j] + grid2.dx1[j] * h
							<< " h " << h << endl ; 
						}
						assert (0) ;
					}
				}
#endif
				return true ;
			}
		}
		//cout << "not found entry" << endl ;
		//cout << "pushing zoid" << endl ;
		if (m_num_vertices > m_zoids.capacity())
		{
			cout << "# nodes of DAG " << m_num_vertices << " exceeds capacity " 				<< m_zoids.capacity() << endl ;
		}
		m_zoids.push_back(zoid_type ()) ;
		//cout << "pushed zoid" << endl ;
		zoid_type & z = m_zoids [m_num_vertices] ;
		z.height = height ;
		//assert (m_num_vertices == m_num_projections) ;
#ifndef NDEBUG
		z.info = grid ;
		z.id = m_num_vertices ;
		//m_num_projections ;
		//assert (m_num_vertices == m_num_projections) ;
		/*cout << "inserting zoid " << z.id << " key " << key << endl ;
		for (int i = N_RANK - 1 ; i >= 0 ; i--)
		{
			cout << " x0 [" << i << "] " << grid.x0 [i] 
			 << " x1 [" << i << "] " << grid.x1 [i] 
			<< " x2 [" << i << "] " << grid.x0[i] + grid.dx0[i] * height
			<< " x3 [" << i << "] " << grid.x1[i] + grid.dx1[i] * height
			<< " h " << height << endl ; 
		}*/
#endif
#ifdef COUNT_PROJECTIONS
		if (N_RANK == 1)
		{
			unsigned long lb = grid.x1 [0] - grid.x0 [0] ;
			unsigned long tb = grid.x1[0] + grid.dx1[0] * height - 
								(grid.x0[0] + grid.dx0[0] * height) ; 	
			unsigned long length ;
			unsigned long index ;
			if (lb > tb)
			{
				length = lb ;
				index = grid.x0 [0] ;
			}
			else
			{
				length = tb ;
				index = grid.x0[0] + grid.dx0[0] * height ;
			}
			index = centroid ;
			//if (length > 0)
			{
				bool found = true ;
				if (lb == 0 || tb == 0)
				{
					/*set <unsigned long> & s = 
						m_1d_index_by_length_triangle [length] ;
					if (s.find(index) == s.end())
					{*/
						//insert if index is not already in the set
						m_num_triangles++ ;
						m_1d_count_proj_length_triangle [length]++ ;
						m_1d_index_by_length_triangle [length].insert(index) ;

					//	found = false ;
					//}
				}			
				else
				{
					/*set <unsigned long> & s = 
						m_1d_index_by_length_trapezoid [length] ;
					if (s.find(index) == s.end())
					{*/
						//insert if index is not already in the set
						m_num_trapezoids++ ;
						m_1d_count_proj_length_trapezoid [length]++ ;
						m_1d_index_by_length_trapezoid [length].insert(index) ;

					//	found = false ;
					//}
				}
				/*if (! found)
				{
					m_1d_count_proj_centroid [centroid]++ ;
				}*/
				//if (lb != 0 && tb != 0)
				{
					if (lb != 0 && tb != 0)
					{
						m_1d_count_proj_centroid [centroid]++ ;
					}
					int i = m_1d_tallest_zoid_per_centroid [centroid] ;
					zoid_type & z1 = m_zoids [i] ;
					if (height > z1.height)
					{
						m_1d_tallest_zoid_per_centroid [centroid] = m_num_vertices ;
					}
					/*else if (height == z1.height)
					{
						grid_info <N_RANK> grid1 = z1.info ;
						unsigned long lb1 = grid1.x1 [0] - grid1.x0 [0] ;
						unsigned long tb1 = grid1.x1[0] + grid1.dx1[0] * height-
								(grid1.x0[0] + grid1.dx0[0] * height) ;
						unsigned long length1 = max(lb1, tb1) ;
						if (lb != 0 && tb != 0 && lb1 != 0 && tb1 != 0 &&
						 abs((int) (length - length1)) > 0)
						{
						cout << " 2 Diff zoids have max height at centroid : "
							<< centroid << endl ;
						cout << " length :  " << length << " length 1 : " <<
							length1 << endl ;
						cout << " index : " << m_num_vertices << " index 1 : " 
							<< i << endl ; 
						for (int i = N_RANK - 1 ; i >= 0 ; i--)
						{
							cout << " x0 [" << i << "] " << grid.x0 [i] 
							 << " x1 [" << i << "] " << grid.x1 [i] 
							<< " x2 [" << i << "] " << grid.x0[i] + grid.dx0[i] * height
							<< " x3 [" << i << "] " << grid.x1[i] + grid.dx1[i] * height
							<< " h " << height << endl ; 
							cout << " x0 [" << i << "] " << grid1.x0 [i] 
							 << " x1 [" << i << "] " << grid1.x1 [i] 
							<< " x2 [" << i << "] " << grid1.x0[i] + grid1.dx0[i] * height
							<< " x3 [" << i << "] " << grid1.x1[i] + grid1.dx1[i] * height
							<< " h " << height << endl ;
						}
						}
					}*/
				}
			}
		}
#endif
		//*zoid = z ;
		//h.insert(std::pair<unsigned long, zoid_type *>(key, z)) ;
		h.insert(std::pair<unsigned long, unsigned long>(key, m_num_vertices)) ;
		//cout << "inserted key" << endl ;
		index = m_num_vertices ;
		//cout << "created zoid " << m_zoids [index].id << endl ;
		m_num_vertices++ ;
		assert (m_num_vertices == m_zoids.size()) ;
		
		/*for (int i = 0 ; i < m_num_vertices ; i++)
		{
			cout << " zoid " << m_zoids [i].id << " address " << & (m_zoids [i]) 
				<< " # children " << m_zoids [i].num_children << endl ;
		}*/
		return false ;
		//}
	}

	void set_clone_array(pochoir_clone_array <N_RANK> * clone_array)
	{
		m_clone_array = clone_array ;
	}

	void dfs(unsigned long node, vector <zoid_type> & temp_zoids,
			 vector<unsigned long> & color, unsigned long & num_vertices)
	{
		color [node] = num_vertices ; //color node gray
		zoid_type & z = m_zoids [node] ;
		if (__builtin_popcount(z.geneity) == 1)
		{
			//do not store node's children
			//cout << "push back zoid " << z.id << endl ;
			temp_zoids.push_back(zoid_type()) ;
			zoid_type & z1 = temp_zoids [num_vertices] ;
			z1.geneity = z.geneity ;
			//z1.num_children = 0 ; //set in constructor
			z1.height = z.height ;
			num_vertices++ ;
			assert (num_vertices == temp_zoids.size()) ;
#ifndef NDEBUG
			z1.info = z.info ;
			z1.id = z.id ;
#endif
		}
		else
		{
			temp_zoids.push_back(z) ; //copy the zoid z
			unsigned long index = num_vertices ; //index into the vector
			num_vertices++ ;
			assert (num_vertices == temp_zoids.size()) ;
			for (int i = 0 ; i < z.num_children ; i++)
			{
				zoid_type & z1 = temp_zoids [index] ;	
				if (color [z.children [i]] == ULONG_MAX) //node is white
				{
					z1.children [i] = num_vertices ;
					dfs(z.children [i], temp_zoids, color, num_vertices) ;
				}
				else
				{
					//node is already visited.
					//assign the child's index 
					z1.children [i] = color [z.children [i]] ;
				}
			}
		}
	}

	//remove children of all homogeneous nodes.
	void compress_dag()
	{
		vector <unsigned long> color ;
		color.reserve(m_num_vertices) ;
		color.resize(m_num_vertices) ;

		vector<zoid_type> temp_zoids ;
		vector<unsigned long> head ;
		unsigned long num_vertices = 0 ;
		for (unsigned long j = 0 ; j < m_num_vertices ; j++)
		{
			color [j] = ULONG_MAX ; //color node white
		}

		for (int j = 0 ; j < m_head.size() ; j++)
		{
			if (color [m_head [j]] == ULONG_MAX)
			{
				head.push_back(num_vertices) ;
				dfs(m_head [j], temp_zoids, color, num_vertices) ;
			}
		}
		//swap the DAG and compressed DAG
		m_zoids.swap(temp_zoids) ;
		m_head.swap(head) ;
		m_num_vertices = num_vertices ;
	}

#ifndef NDEBUG
	void print_dag()
	{
		cout << "# vertices " << m_num_vertices << endl ;
		//cout << "# vertices " << m_num_vertices << " # projections " <<
		//		m_num_projections << endl ;
		//do a BFS of the dag and print each zoid's info	
		vector <unsigned long> color ;
		color.reserve(m_num_vertices) ;
		color.resize(m_num_vertices) ;
		for (int j = 0 ; j < m_head.size() ; j++)
		{
			//if (m_head [j] == 0)
			if (m_head [j] == ULONG_MAX)
			{
				continue ;
			}
			cout << "head " << j << endl ;
			for (int i = 0 ; i < color.size() ; i++)
			{
				color [i] = 0 ;
			}
			
			color [0] = 1 ;
			//deque<zoid_type *> que ;
			deque<unsigned long> que ;
			que.push_front(m_head [j]) ;

			while (!que.empty())
			{
				//zoid_type * z = que.front() ;
				unsigned long index = que.front() ;
				assert (index < m_num_vertices) ;
				zoid_type * z = &(m_zoids[index]) ;
				cout << "\nzoid " << z->id << " height " << z->height <<
					//" num children " << z->children.size() << 
					" num children " << z->num_children << 
					" num_parents " << z->parents.size() << " geneity " ;
					//" num_parents " << z->num_parents << " geneity " ;
				print_bits(&(z->geneity), sizeof(word_type) * 8);
				grid_info <N_RANK> & grid = z->info ;
				int h = z->height ;
				for (int i = N_RANK - 1 ; i >= 0 ; i--)
				{
					cout << " x0 [" << i << "] " << grid.x0 [i] 
					<< " x1 [" << i << "] " << grid.x1 [i] 
					<< " x2 [" << i << "] " << grid.x0[i] + grid.dx0[i] * h
					<< " x3 [" << i << "] " << grid.x1[i] + grid.dx1[i] * h
					<< " h " << h << endl ; 
				}
				if (z->geneity == 0)
				{
					int bottom_volume = 1, top_volume = 1 ;
					for (int i = 0 ; i < N_RANK ; i++)
					{
						int lb = grid.x1[i] - grid.x0[i] ;
						int x0_top = grid.x0[i] + grid.dx0[i] * (h - 1) ;
						int x1_top = grid.x1[i] + grid.dx1[i] * (h - 1) ;
						int tb = x1_top - x0_top ;
						bottom_volume *= lb ;
						top_volume *= tb ;
					}
					if (bottom_volume != 0 || top_volume != 0)
					{
						//zoid is not empty. It must have a geneity
						cout << "Error : geneity is 0 " << endl ;
						assert (z->geneity) ;
					}
				}
				//if (z->num_parents > 1)
				vector<unsigned long> & v = z->parents ;
				cout << "parents " << endl ;
				for (int i = 0 ; i < v.size() ; i++)
				{
					cout << v [i] << " " ;
				}
				cout << endl ;

				que.pop_front() ;
				//cout << "# of children " << z->children.size() << endl ;
				//for (int i = 0 ; i < z->children.size() ; i++)
				for (int i = 0 ; i < z->num_children ; i++)
				{
					//zoid_type * child = z->children[i] ;
					unsigned long index = z->children[i] ;
					cout << "child index " << index << endl ;
					assert (index < m_num_vertices) ;
					zoid_type * child = &(m_zoids [index]);
					if (index && child && color [child->id] == 0)
					{
						color [child->id] = 1 ;
						que.push_back(index) ;
						//que.push_back(child) ;
					}
				}
			}
		}
	}

	void print_heterogeneity()
	{
		set<word_type>::iterator begin = m_heterogeneity.begin() ;
		for (set<word_type>::iterator end = m_heterogeneity.end() ; begin != end ;
																	begin++)
		{
			print_bits(&(*begin), sizeof(word_type) * 8);
		}
	}
#endif

	template <typename F>
	inline void symbolic_abnormal_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, unsigned long,
		int child_index, F const & f) ;

	template <typename F>
	inline void symbolic_abnormal_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, unsigned long,
		int child_index, F const & f) ;

	template <typename F>
	inline void symbolic_abnormal_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, unsigned long, F const & f) ;

	template <typename F>
	inline void symbolic_abnormal_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, unsigned long, F const & f) ;

	template <typename F>
	inline void symbolic_modified_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, unsigned long,
		int child_index, F const & f) ;

	template <typename F>
	inline void symbolic_modified_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, unsigned long,
		int child_index, F const & f) ;

	template <typename F>
	inline void symbolic_modified_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, unsigned long, F const & f, int *) ;

	template <typename F>
	inline void symbolic_modified_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, unsigned long, F const & f, int *) ;

	template <typename F>
	inline void symbolic_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, unsigned long,
		int child_index, F const & f) ;

	template <typename F>
	inline void symbolic_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, unsigned long,
		int child_index, F const & f) ;

	template <typename F>
	inline void symbolic_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, unsigned long, F const & f) ;

	template <typename F>
	inline void symbolic_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, unsigned long, F const & f) ;

	template <typename F, typename BF>
	inline void heterogeneous_modified_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f, BF const & bf) ;

	template <typename F>
	inline void heterogeneous_modified_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, F const & f) ;

	template <typename F, typename BF>
	inline void heterogeneous_modified_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f, BF const & bf) ;
		//F const & f, BF const & bf, int *) ;

	template <typename F>
	inline void heterogeneous_modified_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f) ;
		//F const & f, int *) ;

	template <typename F, typename BF>
	inline void heterogeneous_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f, BF const & bf) ;

	template <typename F>
	inline void heterogeneous_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, F const & f) ;

	template <typename F, typename BF>
	inline void heterogeneous_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f, BF const & bf) ;

	template <typename F>
	inline void heterogeneous_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, F const & f) ;

//#ifndef NDEBUG
#if 0
	template <typename F>
	inline void homogeneous_modified_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f) ;

	template <typename F>
	inline void homogeneous_modified_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, F const & f) ;

	template <typename F>
	inline void homogeneous_modified_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f) ;
		//F const & f, int *) ;

	template <typename F>
	inline void homogeneous_modified_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f) ;
		//F const & f, int *) ;
#else
	template <typename F>
	inline void homogeneous_modified_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, F const & f) ;

	template <typename F>
	inline void homogeneous_modified_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, F const & f) ;

	template <typename F>
	inline void homogeneous_modified_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, F const & f) ;
		//grid_info<N_RANK> const & grid, F const & f, int *) ;

	template <typename F>
	inline void homogeneous_modified_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, F const & f) ;
		//grid_info<N_RANK> const & grid, F const & f, int *) ;
#endif

//#ifndef NDEBUG
#if 0
	template <typename F>
	inline void homogeneous_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f) ;

	template <typename F>
	inline void homogeneous_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, F const & f) ;

	template <typename F>
	inline void homogeneous_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, 
		F const & f) ;

	template <typename F>
	inline void homogeneous_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, zoid_type * projection_zoid, F const & f) ;
#else
	template <typename F>
	inline void homogeneous_space_time_cut_boundary(int t0, int t1,  
		grid_info<N_RANK> const & grid, F const & f) ;

	template <typename F>
	inline void homogeneous_space_time_cut_interior(int t0, int t1, 
		grid_info<N_RANK> const & grid, F const & f) ;

	template <typename F>
	inline void homogeneous_space_cut_boundary(int t0, int t1,
		grid_info<N_RANK> const & grid, F const & f) ;

	template <typename F>
	inline void homogeneous_space_cut_interior(int t0, int t1,
		grid_info<N_RANK> const & grid, F const & f) ;
#endif

	vector<zoid_type> m_zoids ; //the array of all nodes in the DAG
	vector<hash_table> m_projections ; //the array of hashtable of <key, zoid index>
	//zoid_type * m_head [2] ; // the start nodes of the dag
	//unsigned long m_head [2] ; // the indices of start nodes in the dag
	vector<unsigned long> m_head ; // the indices of start nodes in the dag
	Algorithm <N_RANK> & m_algo ; // a reference to Algorithm
	unsigned long m_num_vertices ; //# of zoids in the dag
	//unsigned long m_num_projections ; //# of projections created
	const int NUM_BITS_IN_INT = 8 * sizeof(int) ;
	typedef typename Algorithm<N_RANK>::queue_info queue_info ;
	set<word_type> m_heterogeneity ;
	pochoir_clone_array <N_RANK> * m_clone_array ; 
	int num_bits_width ; //# of bits to store width
	int num_bits_dim ; //# of bits for bottom and top widths in a dimension
#ifdef COUNT_PROJECTIONS
	// keeps a count of each projection length in 1D
	vector<unsigned long> m_1d_count_proj_length_triangle ; 
	vector<unsigned long> m_1d_count_proj_length_trapezoid ; 
	//keeps a count of # of projections whose centroid is a given point
	vector<unsigned long> m_1d_count_proj_centroid ;
	vector<unsigned long> m_1d_tallest_zoid_per_centroid ;

	//set of starting points for each projection length.
    vector<set<unsigned long> > m_1d_index_by_length_triangle ;
    vector<set<unsigned long> > m_1d_index_by_length_trapezoid ;
	unsigned long m_num_triangles ;
	unsigned long m_num_trapezoids ;
#endif

	public :

	heterogeneity(Algorithm<N_RANK> & alg, grid_info<N_RANK> const & grid,
				  bool power_of_two):m_algo(alg)
	{
		m_head.reserve(2) ;
		//m_head.push_back (ULONG_MAX) ;
		//m_head.push_back (ULONG_MAX) ;
		//m_head [0] = ULONG_MAX ;
		//m_head [1] = ULONG_MAX ;
		m_num_vertices = 0 ;
		//m_num_projections = 0 ;
		initialize(grid, power_of_two) ;
		num_bits_dim = sizeof (unsigned long) * 8 / N_RANK ;
		num_bits_width = sizeof (unsigned long) * 8 / (2 * N_RANK) ;
		/*if (N_RANK == 1)
		{
			num_bits_width = sizeof (unsigned long) * 8 / 2 ;
			num_bits_dim = sizeof (unsigned long) * 8 ; 
		}
		else if (N_RANK == 2)
		{
			num_bits_width = sizeof (unsigned long) * 8 / 4 ;
			num_bits_dim = sizeof (unsigned long) * 8 / 2 ; 
		}
		else if (N_RANK == 3)
		{
			num_bits_width = sizeof (unsigned long) * 8 / 6 ;
			num_bits_dim = sizeof (unsigned long) * 8 / 3 ; 
		}*/	
	}


	~heterogeneity()
	{
		//delete all zoids and clear the projections
		destroy_heterogeneity_dag() ;
	}

	template <typename F, typename BF, typename P>
    inline void do_power_of_two_time_cut(int t0, int t1,
        grid_info<N_RANK> const & grid, F const & f, BF const & bf, P const & p)
	{
		int T = t1 - t0 ;
		int W = 0 ;  //max_width among all dimensions
		int slope ;
		for (int i = 0 ; i < N_RANK ; i++)
		{
			if (m_algo.phys_length_ [i] > W)
			{
				W = m_algo.phys_length_ [i] ;
				slope = m_algo.slope_ [i] ;
			}
		}
		struct timeval start, end;
		double dag_time = 0. ;

		//find index of most significant bit that is set
		int Wn = W / (slope << 1) ;
		int index_msb = (sizeof(int) << 3) - __builtin_clz(Wn) - 1 ;
		//h1 = 2^floor(lg(Wn)). The zoid with height h1 undergoes a space cut.
		int h1 = 1 << index_msb ;
		if (T < h1)
		{
			index_msb = (sizeof(int) << 3) - __builtin_clz(T) - 1 ;
			h1 = 1 << index_msb ;
		}
		m_head.push_back (ULONG_MAX) ;
		gettimeofday(&start, 0);
		build_heterogeneity_dag_modified(t0, t0 + h1, grid, p, 0) ;
		int offset = t0 + T / h1 * h1 ;
		int h2 = t1 - offset ;
		int index = 1 ;
		while (h2 >= 1)
		//while (h2 > m_algo.dt_recursive_)
		{
			//find index of most significant bit that is set
			index_msb = (sizeof(int) << 3) - __builtin_clz(h2) - 1 ;
			int h = 1 << index_msb ;
			//cout << "t0 " << t0 << " t1 " << t1 << 
			cout <<	" offset " << offset << " offset + h " <<
				offset + h << " h " << h << endl ;
			m_head.push_back (ULONG_MAX) ;
			build_heterogeneity_dag_modified(offset, offset + h, grid,
											p, index) ;
			offset += h ;
			h2 = t1 - offset ;
			index++ ;
		}
		//do not build dag if h2 = 1. Use the white clone.
		gettimeofday(&end, 0);
		dag_time = tdiff(&end, &start) ;
		cout << "# vertices " << m_num_vertices << endl ;
		cout << "DAG capacity " << m_zoids.capacity() << endl ;
		std::cout << "DAG : consumed time :" << 1.0e3 * dag_time
				<< "ms" << std::endl;
		clear_projections() ;
		//print_dag() ;
#ifdef GENEITY_TEST
		compress_dag () ;
		cout << "# vertices after compression" << m_num_vertices << endl ;
		cout << "DAG capacity after compression" << m_zoids.capacity() << endl ;
#endif
		int m = T / h1 ;
		for (int i = 0 ; i < m ; i++)
		{
			/*cout << "t0 " << t0 << " t1 " << t1 << 
				" h1 " << h1 << " t0 + h1 " <<
				t0 + h1 << endl ;*/
			heterogeneous_modified_space_time_cut_boundary(t0, t0 + h1, grid, 
				&(m_zoids [m_head [0]]), f, bf) ;
			t0 += h1 ;
		}

		h2 = t1 - t0 ;
		index = 1 ;
		//time cuts happen only if height > dt_recursive_
		//while (h2 > m_algo.dt_recursive_)
		while (h2 >= 1)
		{
			//find index of most significant bit that is set
			index_msb = (sizeof(int) << 3) - __builtin_clz(h2) - 1 ;
			int h = 1 << index_msb ;
			cout << "t0 " << t0 << " t1 " << t1 << 
				" h " << h << " t0 + h " <<
				t0 + h << endl ;
			heterogeneous_modified_space_time_cut_boundary(t0, t0 + h, grid, 
				&(m_zoids [m_head [index]]), f, bf) ;
			t0 += h ;
			h2 = t1 - t0 ;
			index++ ;
		}
		/*while (h2 > 1)
		{
			//find index of most significant bit that is set
			index_msb = (sizeof(int) << 3) - __builtin_clz(h2) - 1 ;
			int h = 1 << index_msb ;
			cout << "t0 " << t0 << " t1 " << t1 << 
				" h " << h << " t0 + h " <<
				t0 + h << endl ;
			bool abnormal = false ;
			for (int i = 0 ; i < N_RANK ; i++)
			{
				int num_triangles = m_algo.dx_recursive_ [i] / ((m_algo.slope_ [i] * h) << 1) ;
				m_algo.num_triangles [i] = max(1, num_triangles) ;
				cout << "num_triangles [ " << i << " ] " << m_algo.num_triangles [i] << endl ;
			}
			m_algo.abnormal_region_space_time_cut_boundary(t0, t0 + h, grid, f, bf) ;
			t0 += h ;
			h2 = t1 - t0 ;
		}
		if (h2 == 1)
		{
			cout << "h = 1 t0 " << t0 << " t1 " << t1 << 
				 " t0 + h " << t0 + h2 << endl ;
			//base_case_kernel_boundary(t0, t0 + h2, grid, bf);
			m_algo.shorter_duo_sim_obase_bicut_p(t0, t0 + h2, grid, f, bf) ;
		}*/
	}

	template <typename F, typename BF, typename P>
    inline void do_default_space_time_cuts(int t0, int t1,
        grid_info<N_RANK> const & grid, F const & f, BF const & bf, P const & p)
	{
		int T = t1 - t0 ;
		cout << "t0 " << t0 << " t1 " << t1 << endl ;
		int W = 0 ;  //max_width among all dimensions
		int slope ;
		for (int i = 0 ; i < N_RANK ; i++)
		{
			if (m_algo.phys_length_ [i] > W)
			{
				W = m_algo.phys_length_ [i] ;
				slope = m_algo.slope_ [i] ;
			}		
		}
		struct timeval start, end;
		double dag_time = 0. ;
		if (W >= 2 * slope * T)
		{
			//max width is >= 2 * sigma * h implies the zoid is ready for space cuts
			gettimeofday(&start, 0);
			m_head.push_back (ULONG_MAX) ;
#ifdef COUNT_PROJECTIONS 
			if (N_RANK == 1)
			{
				grid_info<N_RANK> grid2 ;
				int dx = slope * T ;
				grid2.x0[0] = dx ;
            	grid2.dx0[0] = -slope ;
           		grid2.x1[0] = W - dx ;
            	grid2.dx1[0] = slope ;
				cout << "Trap  x0 " << dx << " x1 " << W - dx << " dx0 " <<
					-slope << " dx1 " << slope << endl ;
				build_heterogeneity_dag(t0, t1, grid2, p, 0) ;
				cout << "# triangles " << m_num_triangles <<
					" # of trapezoids " << m_num_trapezoids <<
					" total " << m_num_triangles + m_num_trapezoids <<
					endl ;
			}
#else
			build_heterogeneity_dag(t0, t1, grid, p, 0) ;
#endif
			gettimeofday(&end, 0);
			dag_time = tdiff(&end, &start) ;
			print_dag() ;
			//print_heterogeneity() ;
			cout << "done building dag"  << endl ;
			cout << "# vertices " << m_num_vertices << endl ;
			cout << "DAG capacity " << m_zoids.capacity() << endl ;
			std::cout << "DAG : consumed time :" << 1.0e3 * dag_time
					<< "ms" << std::endl;
			clear_projections() ;
#ifdef GENEITY_TEST
			compress_dag () ;
			cout << "# vertices after compression" << m_num_vertices << endl ;
			cout << "DAG capacity after compression" << m_zoids.capacity() << endl ;
#endif
#ifdef COUNT_PROJECTIONS 
			print_statistics(T) ;
#else
			heterogeneous_space_time_cut_boundary(t0, t1, grid, 
											&(m_zoids [m_head [0]]), f, bf) ;
#endif
		}
		else
		{
			//the zoid will be cut in time until it becomes normal
			//compute 2^k where k = ceil(lg (2 * slope * T / W))
			//int k = 8 * sizeof(int) - __builtin_clz((T - 1) * 2 * slope / W) ;
			//int k = 8 * sizeof(int) - __builtin_clz((T * 2 * slope - 1) / W) ;
			//int k = 8 * sizeof(int) - __builtin_clz(T * 2 * slope / W - 1) ; 
			//cout << "k " << k << endl ;
			//int two_to_the_k = 1 << k ; 
			//cout << "width " << W << " T " << T <<  " 2^k "  << two_to_the_k << endl ;
			//h1 = floor (T / two_to_the_k)
			//int h1 = T / two_to_the_k ;
			cout << "slope " << slope << endl ;
			//choose h1 to be the normalized width
			int h1 = W / (2 * slope) ;
			int h2 = T - T / h1 * h1 ;
			cout << "h1 " << h1 << " h2 " << h2 << endl ;
			struct timeval start, end;
			gettimeofday(&start, 0);
			m_head.push_back (ULONG_MAX) ;
			build_heterogeneity_dag(t0, t0 + h1, grid, p, 0) ;
			gettimeofday(&end, 0);
			dag_time = tdiff(&end, &start) ;
			cout << " t0 + T / h1 * h1  " << t0 + T / h1 * h1 << endl ;
			if (h2 > 0)
			{
				gettimeofday(&start, 0);
				m_head.push_back (ULONG_MAX) ;
				build_heterogeneity_dag(t0 + T / h1 * h1, t1, grid, p, 1) ;
				gettimeofday(&end, 0);
				dag_time += tdiff(&end, &start) ;
			}
			cout << "# vertices " << m_num_vertices << endl ;
			cout << "DAG capacity " << m_zoids.capacity() << endl ;
			std::cout << "DAG : consumed time :" << 1.0e3 * dag_time
					<< "ms" << std::endl;
			clear_projections() ;
			//print_dag() ;
#ifdef GENEITY_TEST
			compress_dag () ;
			cout << "# vertices after compression" << m_num_vertices << endl ;
			cout << "DAG capacity after compression" << m_zoids.capacity() << endl ;
#endif
			print_dag() ;
			int m = T / h1 ;
			for (int i = 0 ; i < m ; i++)
			{
				//cout << "t0 " << t0 << endl ;
				heterogeneous_space_time_cut_boundary(t0, t0 + h1,  
							grid, &(m_zoids [m_head [0]]), f, bf) ;
				t0 += h1 ;
			}
			if (h2 > 0)
			{
				cout << "t0 " << t0 << endl ;
				/*gettimeofday(&start, 0);
				build_heterogeneity_dag(t0, t0 + h2, grid, p, 1) ;
				gettimeofday(&end, 0);
				dag_time += tdiff(&end, &start) ;*/
				//print_dag() ;
				//print_heterogeneity() ;
				heterogeneous_space_time_cut_boundary(t0, t0 + h2,  
							grid, &(m_zoids [m_head [1]]), f, bf) ;
			}
		}
	}
} ;

template<>
template <typename F>
void heterogeneity<1>::compute_geneity(int h, grid_info<1> const & grid, 
					word_type & geneity, F const & f)
{
	int lb = grid.x1[0] - grid.x0[0] ;
	int x0_top = grid.x0[0] + grid.dx0[0] * (h - 1) ;
	int x1_top = grid.x1[0] + grid.dx1[0] * (h - 1) ;
	int tb = x1_top - x0_top ;
	//cout << "comp geneity" << endl ;
	if (lb > tb)
	{
		for (int x0 = grid.x0 [0] ; x0 < grid.x1 [0] ; x0++)
		{
			int index = f(pmod(x0, m_algo.phys_length_ [0])) ;
			set_bit(geneity, index, 1) ;
		}
	}
	else
	{
		for (int x0 = x0_top ; x0 < x1_top ; x0++)
		{
			int index = f(pmod(x0, m_algo.phys_length_ [0])) ;
			set_bit(geneity, index, 1) ;
		}
		
	}
	m_heterogeneity.insert(geneity) ;
}

template<>
template <typename F>
void heterogeneity<2>::compute_geneity(int h, grid_info<2> const & grid, 
					word_type & geneity, F const & f)
{
	//cout << "comp geneity " << endl ;
	int x0 = grid.x0 [1] ;
	int x1 = grid.x1 [1] ;
	int x2 = grid.x0 [1] + grid.dx0 [1] * (h - 1) ;
	int x3 = grid.x1 [1] + grid.dx1 [1] * (h - 1) ;

	int y0 = grid.x0 [0] ;
	int y1 = grid.x1 [0] ;
	int y2 = grid.x0 [0] + grid.dx0 [0] * (h - 1) ; 
	int y3 = grid.x1 [0] + grid.dx1 [0] * (h - 1) ; 
	//cout << "x0 " << x0 << " x1 " << x1 << " x2 " << x2 << " x3 " << x3 << endl ;
	//cout << "y0 " << y0 << " y1 " << y1 << " y2 " << y2 << " y3 " << y3 << endl ;

	if (x0 <= x2 && y0 <= y2)
	{
		//cout << "bottom rectangle " << endl ;
		//bottom rectangle is the projection
		for (int x = x0 ; x < x1 ; x++)
		{
			for (int y = y0 ; y < y1 ; y++)
			{
				int index = f(pmod(x, m_algo.phys_length_ [1]), 
								pmod(y, m_algo.phys_length_ [0])) ;
				set_bit(geneity, index, 1) ;
			}
		}
	}
	else if (x0 > x2 && y0 > y2)
	{
		//cout << "top rectangle " << endl ;
		//top rectangle is the projection
		for (int x = x2 ; x < x3 ; x++)
		{
			for (int y = y2 ; y < y3 ; y++)
			{
				int index = f(pmod(x, m_algo.phys_length_ [1]), 
								pmod(y, m_algo.phys_length_ [0])) ;
				set_bit(geneity, index, 1) ;
			}
		}
	}
	else
	{
		//cout << "octagon " << endl ;
		//projection is an octagon
		if (x0 <= x2)
		{
			//cout << "x dimension converges and y dimension diverges" << endl ;
			//x dimension converges and y dimension diverges
			for (int x = x0, start = y0, end = y1 ; x < x2 ; x++)
			{
				for (int y = start ; y < end ; y++)
				{
					int index = f(pmod(x, m_algo.phys_length_ [1]),
                                pmod(y, m_algo.phys_length_ [0])) ;
					set_bit(geneity, index, 1) ;
				}
				start += grid.dx0 [0];
				end += grid.dx1 [0] ;
			}
			for (int x = x2 ; x < x3 ; x++)
			{
				for (int y = y2 ; y < y3 ; y++)
				{
					int index = f(pmod(x, m_algo.phys_length_ [1]),
                                pmod(y, m_algo.phys_length_ [0])) ;
					set_bit(geneity, index, 1) ;
				}
			}
			for (int x = x3, start = y2, end = y3 ; x < x1 ; x++)
			{
				for (int y = start ; y < end ; y++)
				{
					int index = f(pmod(x, m_algo.phys_length_ [1]),
                                pmod(y, m_algo.phys_length_ [0])) ;
					set_bit(geneity, index, 1) ;
				}
				start -= grid.dx0 [0];
				end -= grid.dx1 [0] ;
			}
		}
		else
		{
			//cout << "x dimension diverges and y dimension converges " << endl ;
			//cout << "x2 " << x2 << " x0 " << x0 << endl ;
			//x dimension diverges and y dimension converges
			for (int x = x2, start = y2, end = y3 ; x < x0 ; x++)
			{
				//cout << "start " << start << " end " << end << endl ;
				for (int y = start ; y < end ; y++)
				{
					int index = f(pmod(x, m_algo.phys_length_ [1]),
                                pmod(y, m_algo.phys_length_ [0])) ;
					//cout << pmod(x, m_algo.phys_length_ [1]) << ", " <<
					//	pmod(y, m_algo.phys_length_ [0]) << endl ;
					set_bit(geneity, index, 1) ;
				}
				start -= grid.dx0 [0];
				end -= grid.dx1 [0] ;
			}
			for (int x = x0 ; x < x1 ; x++)
			{
				for (int y = y0 ; y < y1 ; y++)
				{
					int index = f(pmod(x, m_algo.phys_length_ [1]),
                                pmod(y, m_algo.phys_length_ [0])) ;
					set_bit(geneity, index, 1) ;
				}
			}
			for (int x = x1, start = y0, end = y1 ; x < x3 ; x++)
			{
				//cout << " loop 2 start " << start << " end " << end << endl ;
				for (int y = start ; y < end ; y++)
				{
					int index = f(pmod(x, m_algo.phys_length_ [1]),
                                pmod(y, m_algo.phys_length_ [0])) ;
					//cout << pmod(x, m_algo.phys_length_ [1]) << ", " <<
					//	pmod(y, m_algo.phys_length_ [0]) << endl ;
					set_bit(geneity, index, 1) ;
				}
				start += grid.dx0 [0];
				end += grid.dx1 [0] ;
			}
		}
	}
	m_heterogeneity.insert(geneity) ;
}

#endif

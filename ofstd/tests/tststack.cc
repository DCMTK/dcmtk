#include <iostream.h>
#include "ofstack.h"

int main()
{
    OFStack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);
    
    OFStack<int> nst(st);
    cout << "Output of number of Elements in st: " << st.size() << endl;
    cout << "Output and deletion of st: ";
    while(!st.empty())
    {
	cout << st.top() << " ";
	st.pop();
    }
    cout << endl;

    cout << "Output of number of Elements in copy from st: " << nst.size() << endl;
    cout << "Output and deletion of copy from st: ";
    while(!nst.empty())
    {
	cout << nst.top() << " ";
	nst.pop();
    }
    cout << endl;
}

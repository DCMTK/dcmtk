#include <iostream.h>
#include "ofstack.h"

int main()
{
    OFStack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);
    while(!st.empty())
    {
	cout << "Wert: " << st.top() << endl;
	st.pop();
    }
}

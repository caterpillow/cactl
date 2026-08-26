// Compile check for DynamicBitset.h (template recursion + assert).
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/DynamicBitset.h"

int main() {
    solve<>(1);
    solve<>(1000);
    solve<>(MAX_LEN);
    cout << "Tests passed!" << endl;
}

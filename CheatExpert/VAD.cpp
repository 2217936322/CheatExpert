#include <ntddk.h>
#include <windef.h>
#include "public.h"
#include "struct10.h"


TABLE_SEARCH_RESULT
MiFindNodeOrParent(
    IN PMM_AVL_TABLE Table,
    IN ULONG_PTR StartingVpn,
    OUT PMMADDRESS_NODE* NodeOrParent
)

/*++

    Routine Description:

        This routine is used by all of the routines of the generic
        table package to locate the a node in the tree.  It will
        find and return (via the NodeOrParent parameter) the node
        with the given key, or if that node is not in the tree it
        will return (via the NodeOrParent parameter) a pointer to
        the parent.

    Arguments:

        Table - The generic table to search for the key.

        StartingVpn - The starting virtual page number.

        NodeOrParent - Will be set to point to the node containing the
        the key or what should be the parent of the node
        if it were in the tree.  Note that this will *NOT*
        be set if the search result is TableEmptyTree.

    Return Value:

        TABLE_SEARCH_RESULT - TableEmptyTree: The tree was empty.  NodeOrParent
        is *not* altered.

        TableFoundNode: A node with the key is in the tree.
        NodeOrParent points to that node.

        TableInsertAsLeft: Node with key was not found.
        NodeOrParent points to what would
        be parent.  The node would be the
        left child.

        TableInsertAsRight: Node with key was not found.
        NodeOrParent points to what would
        be parent.  The node would be
        the right child.

    Environment:

        Kernel mode.  The PFN lock is held for some of the tables.

--*/

{
    PMMADDRESS_NODE Child;
    PMMADDRESS_NODE NodeToExamine;
    PMMVAD_SHORT    VpnCompare;
    ULONG_PTR       startVpn;
    ULONG_PTR       endVpn;

    if (Table->NumberGenericTableElements == 0) {
        return TableEmptyTree;
    }

    NodeToExamine = (PMMADDRESS_NODE)GET_VAD_ROOT(Table);

    for (;;) {

        VpnCompare = (PMMVAD_SHORT)NodeToExamine;
        startVpn = VpnCompare->StartingVpn;
        endVpn = VpnCompare->EndingVpn;

#if defined( _WIN81_ ) || defined( _WIN10_ )
        startVpn |= (ULONG_PTR)VpnCompare->StartingVpnHigh << 32;
        endVpn |= (ULONG_PTR)VpnCompare->EndingVpnHigh << 32;
#endif  

        //
        // Compare the buffer with the key in the tree element.
        //

        if (StartingVpn < startVpn) {

            Child = NodeToExamine->LeftChild;

            if (Child != NULL) {
                NodeToExamine = Child;
            }
            else {

                //
                // Node is not in the tree.  Set the output
                // parameter to point to what would be its
                // parent and return which child it would be.
                //

                *NodeOrParent = NodeToExamine;
                return TableInsertAsLeft;
            }
        }
        else if (StartingVpn <= endVpn) {

            //
            // This is the node.
            //

            *NodeOrParent = NodeToExamine;
            return TableFoundNode;
        }
        else {

            Child = NodeToExamine->RightChild;

            if (Child != NULL) {
                NodeToExamine = Child;
            }
            else {

                //
                // Node is not in the tree.  Set the output
                // parameter to point to what would be its
                // parent and return which child it would be.
                //

                *NodeOrParent = NodeToExamine;
                return TableInsertAsRight;
            }
        }

    };
}

NTSTATUS BBFindVAD(IN PEPROCESS pProcess, IN ULONG_PTR address, OUT PMMVAD_SHORT* pResult, IN int VadRootOffset)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG_PTR vpnStart = address >> PAGE_SHIFT;

    ASSERT(pProcess != NULL && pResult != NULL);
    if (pProcess == NULL || pResult == NULL)
        return STATUS_INVALID_PARAMETER;

    if (VadRootOffset == 0)
    {
        PrintLog("[%s]: Invalid VadRoot offset\n", __FUNCTION__);
        status = STATUS_INVALID_ADDRESS;
    }


    PMM_AVL_TABLE pTable = (PMM_AVL_TABLE)((PUCHAR)pProcess + VadRootOffset);
    PMM_AVL_NODE pNode = GET_VAD_ROOT(pTable);

    // Search VAD
    if (MiFindNodeOrParent(pTable, vpnStart, &pNode) == TableFoundNode)
    {
        *pResult = (PMMVAD_SHORT)pNode;
    }
    else
    {
        PrintLog("[%s]: VAD entry for address 0x%p not found\n", __FUNCTION__, address);
        status = STATUS_NOT_FOUND;
    }

    return status;
}
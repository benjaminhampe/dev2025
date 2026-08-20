/*
InsertMenuW needs all four parameters because Windows must know which menu, where to insert, how to interpret the index, what command ID to assign, and what text to display. Each parameter directly maps to how Explorer builds your context‑menu entry.

🧩 Parameter breakdown (why each is required)

1. hMenu — the actual menu handle

    This is the HMENU that Explorer passes to your shell extension inside IContextMenu::QueryContextMenu.

    Windows doesn’t know which menu you want to modify unless you tell it.
    hMenu is the live menu object that Explorer is currently constructing.

    Without it, there is no menu to insert into.

2. indexMenu — position where the item should be inserted

    This is the zero‑based position inside the menu.

    Example:
        0 → top of the menu
        5 → after the fifth item
        -1 (or GetMenuItemCount(hMenu)) → append at the bottom

    This matters because Explorer merges multiple handlers (ZIP tools, cloud sync apps, etc.) and each extension must specify exact placement.

3. MF_BYPOSITION — interpret indexMenu as a position, not a command ID

    InsertMenuW can insert by:
        position → MF_BYPOSITION
        command ID → MF_BYCOMMAND

    You must specify one, otherwise Windows cannot know whether indexMenu is:
        a menu index, or
        an existing command ID to insert before.

    Shell extensions almost always use MF_BYPOSITION because Explorer gives you a position to start from.

4. idCmdFirst — the command ID assigned to your menu item

    This is critical for shell extensions.
    Explorer gives your extension a range of command IDs:

    idCmdFirst … idCmdLast

    You must assign one of these IDs to each menu item you insert.
    Later, when the user clicks your item, Explorer calls:
    Code

    InvokeCommand(CMINVOKECOMMANDINFO* pInfo)

    Inside pInfo->lpVerb, you receive the same command ID you inserted.
    If you insert a menu item without a valid command ID, Explorer cannot route the click back to your extension.

5. L"8‑Zip Compress ShellExtension" — the visible menu text

    This is simply the Unicode string shown in the context menu.

    It must be provided because:
        Windows does not auto‑generate text
        Shell extensions often localize this string
        Explorer needs a human‑readable label to display

🧩 Why all parameters matter together

    Explorer builds menus by merging:
        built‑in items
        static registry verbs
        COM shell extensions
        cloud sync providers
        ZIP tools
        image editors
        etc.

    Your extension must tell Explorer:
    Parameter       What Explorer learns
    hMenu           Which menu to modify
    indexMenu       Where to insert
    MF_BYPOSITION	How to interpret the index
    idCmdFirst      Which command ID to assign
    "8‑Zip Compress ShellExtension"	What text to display

    Without any one of these, Explorer cannot correctly integrate your menu item.

    If you want, I can also show the correct pattern for multiple items, submenus, or dynamic enable/disable logic inside QueryContextMenu.

    WINUSERAPI WINBOOL WINAPI
    InsertMenu( HMENU hMenu,
                UINT uPosition,
                UINT uFlags,
                UINT_PTR uIDNewItem,
                LPCWSTR lpNewItem);
*/

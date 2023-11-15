Coding style
--------------

We shall be using camelCase and clang.format rules to format the code, this is provided with the repo.

## pointers
pointers should be prefixed with `p` eg.  

pMyClass, pRXbuffer etc.

## structs
prefix the struct definition with `s` eg.  

```
struct sMyStruct {...};
```
## classes
prefix the class definition with `c` eg.  

```
class cMyClass {...};
```

## class variables
prefix with `_` eg.  

```
cMyclass {

  ...
  private:
  int _myCounter = 0;
};
```

## Other notes
- use `constexpr` over `#define`, in fact keep `#define` to absolute minimum and capitals for constants:
```
constexpr int CONST_WONDER_OF_THE_UNIVERSE = 42;
```
- use `using` over `typedef`

# GIT
We shall be Signing-off under the commits and use widely used open source standards [2]  

One should use a default message in git commit [1] eg.  

```
module: title of commit

Commit message

Signed-off:
Reviewed-by:
etc.
```

Rebase your patches before committing, there is no excuse for:  

commit 1: postgresql: added-new-feature  
commit 2: postgresql: oh-it-was-not-working  
commit 3: postgresql: now-it-actually-works  
commit 4: postgresql: I hate coding, it does not actually work  
commit 5: postgresql: I love coding, it does finally work  

fixup the above N patches (change pick to f) into a single commit using the command:  

```
$ git rebase -i HEAD~N
```

Note that after rebasing you will need to force push to your branch.  

[1] https://gist.github.com/lisawolderiksen/a7b99d94c92c6671181611be1641c733

[2] http://web.archive.org/web/20160507011446/http://gerrit.googlecode.com/svn/documentation/2.0/user-signedoffby.html
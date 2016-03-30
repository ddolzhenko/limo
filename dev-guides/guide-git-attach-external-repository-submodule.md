
# **Do not use them for developement of a submodule itself**

# Process
* Creates a new repo
* Then clones another repo as submodule
* Then we have that submodule use a tag called V3.1.2
* And then we commit
* And that looks a little bit like this:

git init 
vi README
git add README
git commit 
git submodule add git://github.com/XXXXX/xxx.yyyy.git stm32_std_lib
git status

git submodule init
git submodule update

cd stm32_std_lib/
git reset --hard V3.1.2 
cd ..
git commit -a

git submodule status

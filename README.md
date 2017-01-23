# c379_w17
Repo for CMPUT 379 - Winter 2017

## Pull Request
All changes to the code in the future should be submitted as a PR, and each PR should be reviewed and tested before being merge to the master branch.

## Contribution Procedure
1. Fork the repo at https://github.com/sdlarsen1/c379_w17 to your own Github repo.  

2. Make a local clone of your fork on your own computer.   
` git clone https://github.com/username/Unter `  

3. Add the original repo as new remote.  
`git remote add upstream https://github.com/sdlarsen1/c379_w17`

4. Fetch changes to local.  
`git fetch upstream`

5. Checkout back to master.   
`git checkout master`

6. If you want your changes in the feature branch to always the latest.  
 `git rebase upstream/master`

7. if you want to reflect the true ordering of commits.  
 `git merge upstream/master`

8. After you are done with your work, push the changes to your own fork to Github, and then submit a PR at the team repo.  
 `git push origin master`

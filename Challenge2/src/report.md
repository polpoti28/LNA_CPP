# Challenge 2

In our second challenge, we explored spectral clustering, a technique that partitions networks by analyzing the eigenvalues and eigenvectors of their associated matrices. We applied this strategy to two case studies: first, a small illustrative graph where the clustering can be visually verified, and second, a real-world Facebook friendship network containing 351 users

## Small Graph

First, we applied our clustering strategy to the graph shown in the picture below.


<p align="center">
    <img src="./Pictures/challenge_small_graph.png" alt=" example graph " width="300">
</p>

We computed the Laplacian graph by performing :
$$ L_g = D_g - A_g $$
Where : 
- $A_g$ is the adjacency matrix of the graph
-  $D_g$ is is diagonal matrix with the vector $v_g$ on the main diagonal and zeros everywhere 
else.
- $v_g$ is a vector such that each component $v_i$ is the sum of the entries in the i-th
row of matrix $A_g$

We then computed the **Fiedler vector** which is the eigenvector corresponding to the second smallest eigenvalue of the Laplacian graph $L_g$

The Fiedler vector obtained for the small example is:

```
0.455542
0.391696
0.218053
0.391696
-0.190354
-0.308704
-0.340521
-0.308704
-0.308704
```

We computed this vector in two different ways : 

- We used the SelfAdjoint solver provived by the Eigen library to compute all the eiganvalue and eigenvector the we choose the second smallest 
- We constructed a new matrix B by deflating the matrix A, removing the smallest eigenvalue, by performing an HouseHolder transformation


 
The Fiedler vector partitions the graph into two subgraphs, which, based on the sign of the entries, are formed by nodes **{1, 2, 3, 4}** and **{5, 6, 7, 8, 9}**.  
This result is consistent with the structure of the graph shown in the reference image.




## Facebook Community Graph

We then moved to the larger graph and performed the same strategy now using methods provived by **LIS** library.  
We computed the **largest eigenvalue** of its Laplacian matrix using the  the LIS **Power Method** as an iterative solver.  
A **shift of 29.5** was chosen, as it provided the fastest convergence.

To compute the **second smallest eigenvalue**, we used the **Inverse Power Method** in LIS, specifying the option `-ss 2` to compute both of the two smallest eigenvalues.

---

## Clustering Results

By analyzing the sign of the entries in the eigenvector corresponding to the second smallest eigenvalue, we observed a clear partition into two clusters of sizes **52** and **299**, representing two distinct communities.

After constructing the **permutation matrix** and the **ordered adjacency matrix** $A_{\text{ord}}$
, we observed that the **off-diagonal blocks** of $A_{\text{ord}}$
 represent the **connections between the two clusters**.  
Therefore, we expected fewer nonzero elements in these blocks compared to the off-diagonal blocks of the **non-ordered** adjacency matrix $A_{\text{s}}$


This expectation was confirmed by our results:

- **332 nonzero elements** for $A_{\text{ord}}$

- **1162 nonzero elements** for $A_{\text{s}}$


This significant reduction indicates that the clustering strategy successfully separated the graph into two weakly connected communities.

---

## Visualization

Below are the sparsity patterns of the two matrices:



| social.mtx  | ordered.mtx |
|------------|----------|
| ![Alt1](./Pictures/A_s_full_mat.png) | ![Alt2](./Pictures/A_ord_full_mat.png) |


## off-diagonal matrices 

| social.mtx  | 
|------------|
| ![Alt1](./Pictures/A_S_off_diag.png)|

|ordered.mtx |
|----------|
 ![Alt2](./Pictures/A_ord_off_diag.png) |

---

## Conclusion

The spectral clustering based on the Fiedler vector correctly identified two distinct communities in the network.  
The reduction of nonzero entries in the off-diagonal blocks of the reordered adjacency matrix confirms that the clusters are weakly connected, validating the effectiveness of the method.

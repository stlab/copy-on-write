# GitHub Pages Setup

This document describes how to set up GitHub Pages for automatic documentation deployment.

## Repository Settings

To enable GitHub Pages deployment via GitHub Actions:

1. Go to your repository on GitHub
2. Click on **Settings** → **Pages**
3. Under **Source**, select **GitHub Actions**
4. Save the changes

## Workflow Permissions

The CI workflow requires specific permissions to deploy to GitHub Pages:

1. Go to **Settings** → **Actions** → **General**
2. Under **Workflow permissions**, ensure:
   - **Read and write permissions** is selected, OR
   - **Read repository contents and packages permissions** is selected with additional permissions for:
     - Contents: Read
     - Pages: Write
     - ID Token: Write

## First Deployment

Once the workflow runs successfully on the `main` branch:

1. The documentation will be available at: `https://[username].github.io/[repository-name]/`
2. For this repository: `https://stlab.github.io/copy-on-write/`

## Troubleshooting

### Pages Not Deploying
- Check that the workflow completed successfully in the **Actions** tab
- Verify that GitHub Pages is configured to use **GitHub Actions** as the source
- Ensure the workflow has the necessary permissions

### 404 Error
- Wait a few minutes after the first deployment
- Check that the `docs` job completed successfully and uploaded the artifact

### Permission Errors
- Verify that the repository has Pages enabled
- Check that the workflow has `pages: write` and `id-token: write` permissions

## Manual Testing

You can test the documentation build locally:

```bash
# Build documentation
cmake --preset test -DBUILD_DOCS=ON
cmake --build build/test --target docs

# View locally
open build/test/docs/html/index.html
``` 

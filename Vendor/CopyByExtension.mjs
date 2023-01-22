import { mkdirSync, copyFileSync, readdirSync, statSync } from "node:fs";
import { join, dirname, basename, extname } from "node:path";

const origDir = "C:\\Users\\Michael\\Downloads\\glm\\glm";
const newDir = "C:\\Users\\Michael\\Downloads\\Nippon\\Vendor\\Glm";

const extensions = [".h", ".c", ".hpp", ".cpp", ".inl"];

const getFilesRecursive = (dir, array) =>
{
  array = array || [];
  readdirSync(dir).forEach((file) =>
  {
    if (statSync(dir + "/" + file).isDirectory())
    {
      array = getFilesRecursive(dir + "/" + file, array);
    }
    else
    {
      array.push(join(dir, "/", file));
    }
  });
  return array;
}

getFilesRecursive(origDir).forEach((file) =>
{
  const dir = dirname(file);
  const name = basename(file);
  const ext = extname(file);
  if (extensions.includes(ext))
  {
    mkdirSync(newDir + dir.replace(origDir, "") + "\\", { recursive: true }, (err) => {});
    copyFileSync(file, newDir + dir.replace(origDir, "") + "\\" + name);
  }
});
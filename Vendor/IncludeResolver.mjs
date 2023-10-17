import { writeFileSync, mkdirSync, readFileSync, readdirSync, statSync } from "node:fs";
import { join, resolve, dirname, basename, extname } from "node:path";

//const origDir = "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.0.1\\include";
//const newDir = "C:\\Users\\Michael\\Downloads\\Nippon\\Vendor\\FbxSdk";

const origDir = "C:\\Users\\Michael\\Downloads\\bullet3\\src";
const newDir = "C:\\Users\\Michael\\Downloads\\Nippon\\Vendor\\Bullet";

//const pathPrefix = "Vendor/FbxSdk";
const pathPrefix = "Vendor/Bullet";

const extensions = [".h", ".c", ".hpp", ".cpp"];

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
const getFiles = (dir, array) =>
{
	array = array || [];
	readdirSync(dir).forEach((file) =>
	{
		array.push(file);
	});
	return array;
}
const resolveIncludes = (dir, data, regex, leftClosure, rightClosure) =>
{
	let matches = [];
	let match = regex.exec(data);
	while (match != null)
	{
		matches.push(match[0]);
		match = regex.exec(data);
	}
	for (let j = 0; j < matches.length; ++j)
	{
		if (getFiles(dir).includes(matches[j].replace(dir, "")))
		{
			const relativePath = resolve(dir).replace(origDir, "").replaceAll("\\", "/") + "/" + matches[j];
			data = data.replace(leftClosure + matches[j] + rightClosure, "<" + pathPrefix + relativePath + ">");
		}
		else if (getFiles(origDir).includes(matches[j].substr(0, matches[j].indexOf("/"))))
		{
			data = data.replace(leftClosure + matches[j] + rightClosure, "<" + pathPrefix + "/" + matches[j] + ">");
		}
	}
	return data;
}

getFilesRecursive(origDir).forEach((file) =>
{
	const dir = dirname(file);
	const name = basename(file);
	const ext = extname(file);
	if (extensions.includes(ext))
	{
		let data = readFileSync(file, "utf8");
		data = resolveIncludes(dir, data, /(?<=(#include.?\")).+(?=(\"))/g, "\"", "\"");
		data = resolveIncludes(dir, data, /(?<=(#include.?\<)).+(?=(\>))/g, "<", ">");
		mkdirSync(newDir + dir.replace(origDir, "") + "\\", { recursive: true }, (err) => {});
		writeFileSync(newDir + dir.replace(origDir, "") + "\\" + name, data, (err) => {});
	}
});
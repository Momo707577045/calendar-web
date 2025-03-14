const fs = require('fs');
const path = require('path');

// 读取 generate.ts 文件内容
const filePath = path.join(__dirname, 'generate.ts');
const fileContent = fs.readFileSync(filePath, 'utf8');

// 用于存储提取的日期
const holidays = [];
const workdays = [];

// 正则表达式匹配所有年份的定义
// 匹配从 arrangement.y(YYYY) 开始，到下一个 arrangement.y 或 return 语句之前的所有内容
const yearRegex = /arrangement\.y\((\d{4})\)([\s\S]*?)(?=arrangement\.y\(|return)/g;

// 匹配放假日期的正则表达式 (.r 方法调用)
// 匹配 .r(M, D) 和可选的 .to(M, D) 部分
const holidayRegex = /\.r\((\d{1,2}),\s*(\d{1,2})\)(?:\.to\((\d{1,2}),\s*(\d{1,2})\))?/g;

// 匹配工作日的正则表达式 (.w 方法调用)
// 匹配 .w(M, D) 和可选的 .to(M, D) 部分
const workdayRegex = /\.w\((\d{1,2}),\s*(\d{1,2})\)(?:\.to\((\d{1,2}),\s*(\d{1,2})\))?/g;

/**
 * 生成指定日期范围内的所有日期（包括起始日期和结束日期）
 * @param {number} year - 年份
 * @param {number} startMonth - 开始月份
 * @param {number} startDay - 开始日期
 * @param {number} endMonth - 结束月份
 * @param {number} endDay - 结束日期
 * @returns {string[]} - YYYY-MM-DD 格式的日期数组
 */
function generateDateRange(year, startMonth, startDay, endMonth, endDay) {
  const dates = [];

  // 创建起始日期和结束日期
  const startDate = new Date(year, startMonth - 1, startDay);
  const endDate = new Date(year, endMonth - 1, endDay);

  // 从起始日期开始，直到结束日期
  let currentDate = new Date(startDate);

  while (currentDate <= endDate) {
    const currentYear = currentDate.getFullYear();
    const currentMonth = (currentDate.getMonth() + 1).toString().padStart(2, '0');
    const currentDay = currentDate.getDate().toString().padStart(2, '0');

    dates.push(`${currentYear}-${currentMonth}-${currentDay}`);

    // 移动到下一天
    currentDate.setDate(currentDate.getDate() + 1);
  }

  return dates;
}

// 遍历所有年份定义
let match;
while ((match = yearRegex.exec(fileContent)) !== null) {
  const year = match[1];
  const yearContent = match[2];

  // 提取放假日期
  let holidayMatch;
  while ((holidayMatch = holidayRegex.exec(yearContent)) !== null) {
    const month = holidayMatch[1];
    const day = holidayMatch[2];

    // 格式化单日假期
    const formattedDate = `${year}-${month.padStart(2, '0')}-${day.padStart(2, '0')}`;
    holidays.push(formattedDate);

    // 如果有日期范围 (.to 方法)
    if (holidayMatch[3] && holidayMatch[4]) {
      const endMonth = holidayMatch[3];
      const endDay = holidayMatch[4];

      // 生成日期范围内的所有日期（不包括已添加的起始日期）
      const dateRange = generateDateRange(
        parseInt(year),
        parseInt(month),
        parseInt(day) + 1, // 从起始日期的下一天开始
        parseInt(endMonth),
        parseInt(endDay)
      );

      // 添加到假期数组
      holidays.push(...dateRange);
    }
  }

  // 提取工作日
  let workdayMatch;
  while ((workdayMatch = workdayRegex.exec(yearContent)) !== null) {
    const month = workdayMatch[1];
    const day = workdayMatch[2];

    // 格式化单日工作日
    const formattedDate = `${year}-${month.padStart(2, '0')}-${day.padStart(2, '0')}`;
    workdays.push(formattedDate);

    // 如果有日期范围 (.to 方法)
    if (workdayMatch[3] && workdayMatch[4]) {
      const endMonth = workdayMatch[3];
      const endDay = workdayMatch[4];

      // 生成日期范围内的所有日期（不包括已添加的起始日期）
      const dateRange = generateDateRange(
        parseInt(year),
        parseInt(month),
        parseInt(day) + 1, // 从起始日期的下一天开始
        parseInt(endMonth),
        parseInt(endDay)
      );

      // 添加到工作日数组
      workdays.push(...dateRange);
    }
  }
}

// 去重并排序
const uniqueHolidays = [...new Set(holidays)].sort();
const uniqueWorkdays = [...new Set(workdays)].sort();

// 创建结果对象
const result = {
  holidays: uniqueHolidays,
  workdays: uniqueWorkdays
};

// 写入 JSON 文件
fs.writeFileSync('holidays-workdays.json', JSON.stringify(result, null, 2), 'utf8');

console.log('提取完成！数据已保存到 holidays-workdays.json 文件中');
console.log(`共提取到 ${uniqueHolidays.length} 个假期和 ${uniqueWorkdays.length} 个工作日`); 
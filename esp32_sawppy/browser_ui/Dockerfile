FROM node:18-slim
WORKDIR /browser_ui
COPY package.json yarn.lock /browser_ui/
RUN yarn install
CMD ["yarn", "run", "start"]

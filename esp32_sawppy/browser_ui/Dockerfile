FROM node:14
WORKDIR /browser_ui
COPY package.json yarn.lock ./
RUN yarn install
CMD ["yarn", "run", "start"]

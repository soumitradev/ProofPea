#include <debug/ast.h>

namespace debug {
namespace ast {

GVC_t *graphvizInit() {
  // TODO: Track errors in this function
  logger::Logger::dispatchLog(logger::debugLog{"Creating graphviz context"});
  GVC_t *ctx = gvContext();
  const char *astFilename = "ast.svg";

  // Create the AST output file
  logger::Logger::dispatchLog(logger::debugLog{"Creating AST svg output file"});
  std::ofstream astFile(astFilename);
  astFile.close();

  // Pass args to graphviz
  logger::Logger::dispatchLog(
      logger::debugLog{"Configuring the graphviz context"});
  const char *args[] = {"dot", "-Tsvg", "-o", (char *)astFilename};
  gvParseArgs(ctx, 4, (char **)args);

  logger::Logger::dispatchLog(
      logger::infoLog{"graphviz has been initialized successfully"});
  return ctx;
}

void graphvizClose(GVC_t *ctx, Agraph_t *graph) {
  // Compute, render and write the graph according to args passed to
  // graphvizContext
  // TODO: Track errors in this function
  logger::Logger::dispatchLog(
      logger::debugLog{"Rendering graphviz layouts and content"});
  gvLayoutJobs(ctx, graph);
  gvRenderJobs(ctx, graph);

  // Deallocate data
  logger::Logger::dispatchLog(logger::debugLog{"Deallocating graphviz data"});
  gvFreeLayout(ctx, graph);
  agclose(graph);

  gvFreeContext(ctx);
  logger::Logger::dispatchLog(
      logger::infoLog{"graphviz has been uninitialized successfully"});
}

Agnode_t *renderNode(Agraph_t *graph, void *nodePtr, const std::string lexeme,
                     parser::parser::NodeType type) {
  // TODO: Track errors in this function
  char *name = nullptr;
  if (type == parser::parser::ATOM || type == parser::parser::ABSOLUTE) {
    // Use the lexeme of the atom itself, since atoms with the same lexeme are
    // the same
    logger::Logger::dispatchLog(
        logger::debugLog{"Generating name for atom/absolute node"});
    name = (char *)lexeme.data();
  } else {
    // Use the pointer to the operator to make a unique name for the operator
    logger::Logger::dispatchLog(
        logger::debugLog{"Generating name for operator node"});
    std::ostringstream pointerStream;
    pointerStream << nodePtr;
    name = pointerStream.str().data();
  }

  Agnode_t *renderedNode = agnode(graph, name, 1);

  // Set the display label to the lexeme of the node
  logger::Logger::dispatchLog(
      logger::debugLog{"Setting display label of node \"" + lexeme + "\""});
  if (lexeme != "->") {
    agsafeset(renderedNode, (char *)"label", (char *)lexeme.data(), (char *)"");
  } else {
    // Set a nice readable symbol for implies
    agsafeset(renderedNode, (char *)"label", (char *)"⇒", (char *)"");
  }

  logger::Logger::dispatchLog(
      logger::debugLog{"Setting display color of node \"" + lexeme + "\""});
  // Render atoms in red
  if (type == parser::parser::ATOM) {
    agsafeset(renderedNode, (char *)"color", (char *)"red", (char *)"");
  }
  // Render absolutes in blue
  if (type == parser::parser::ABSOLUTE) {
    agsafeset(renderedNode, (char *)"color", (char *)"blue", (char *)"");
  }

  return renderedNode;
}

void renderASTRecursive(const Agraph_t *graph, const parser::parser::Node *node,
                        const Agnode_t *parent, bool isLeftChild) {
  // TODO: Track errors in this function
  Agnode_t *renderedNode = nullptr;
  if (node->type == parser::parser::UNARY) {
    const auto unaryOperatorNode =
        std::get<const parser::parser::UnaryOperator *>(node->node);

    logger::Logger::dispatchLog(logger::debugLog{
        "Rendering unary operator \"" + unaryOperatorNode->op->lexeme +
        "\" at position " + std::to_string(unaryOperatorNode->op->position)});
    renderedNode = renderNode((Agraph_t *)graph, (void *)unaryOperatorNode,
                              unaryOperatorNode->op->lexeme, node->type);
    renderASTRecursive(graph, unaryOperatorNode->child, renderedNode, false);
  } else if (node->type == parser::parser::BINARY) {
    const auto binaryOperatorNode =
        std::get<const parser::parser::BinaryOperator *>(node->node);

    logger::Logger::dispatchLog(logger::debugLog{
        "Rendering binary operator \"" + binaryOperatorNode->op->lexeme +
        "\" at position " + std::to_string(binaryOperatorNode->op->position)});

    renderedNode = renderNode((Agraph_t *)graph, (void *)binaryOperatorNode,
                              binaryOperatorNode->op->lexeme, node->type);
    renderASTRecursive(graph, binaryOperatorNode->left, renderedNode, true);
    renderASTRecursive(graph, binaryOperatorNode->right, renderedNode, false);
  } else if (node->type == parser::parser::ATOM) {
    const auto atomNode = std::get<const parser::parser::Atom *>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{
        "Rendering atom \"" + atomNode->token->lexeme + "\" at position " +
        std::to_string(atomNode->token->position)});
    renderedNode = renderNode((Agraph_t *)graph, (void *)atomNode,
                              atomNode->token->lexeme, node->type);
  } else if (node->type == parser::parser::ABSOLUTE) {
    const auto absoluteNode =
        std::get<const parser::parser::Absolute *>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{
        "Rendering absolute \"" + absoluteNode->token->lexeme +
        "\" at position " + std::to_string(absoluteNode->token->position)});
    renderedNode = renderNode((Agraph_t *)graph, (void *)absoluteNode,
                              absoluteNode->token->lexeme, node->type);
  }

  if (parent == nullptr) return;
  std::string parentLabel = agget((void *)parent, (char *)"label");
  std::string currentLabel = agget((void *)renderedNode, (char *)"label");
  logger::Logger::dispatchLog(logger::debugLog{"Rendering edge from parent \"" +
                                               parentLabel + "\" to \"" +
                                               currentLabel + "\""});
  auto edge = agedge((Agraph_t *)graph, (Agnode_t *)parent, renderedNode, 0, 1);
  if (parentLabel == "⇒") {
    agsafeset((void *)edge, (char *)"label", (char *)(isLeftChild ? "L" : "R"),
              (char *)"");
  }
}

void printAST(const parser::parser::AST *ast) {
  // TODO: Track errors in this function
  const auto ctx = graphvizInit();

  logger::Logger::dispatchLog(logger::infoLog{"Starting the AST render"});
  Agraph_t *astGraph = agopen((char *)"g", Agdirected, 0);
  renderASTRecursive(astGraph, ast->root, nullptr, false);

  graphvizClose(ctx, astGraph);
}
}  // namespace ast
}  // namespace debug
